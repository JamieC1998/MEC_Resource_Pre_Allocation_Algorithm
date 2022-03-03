//
// Created by Jamie Cotter on 26/01/2022.
//

#include "AlgorithmServices.h"
#include "../../utils/HelperFunctions/HelperFunctions.h"
#include "../NetworkTopologyServices/NetworkTopologyServices.h"
#include "../../Constants/ReactiveConstants.h"
#include "../../Constants/PreallocationConstants.h"
#include "../../Constants/AlgorithmMode/AlgorithmMode.h"
#include "../PreallocationFunctions/PreallocationFunctions.h"
#include "../PartitionFunctions/PartitionFunctions.h"

void AlgorithmServices::runAlgorithm(std::vector<std::shared_ptr<Task>> &readyTaskList,
                                     std::vector<std::shared_ptr<Graph<Task, bool>>> &applications,
                                     std::vector<std::shared_ptr<TaskMapping>> &inProgress,
                                     Graph<ComputationNode, std::shared_ptr<EdgeData>> &network_graph,
                                     std::vector<std::shared_ptr<TaskMapping>> &reservationQueue, float time,
                                     std::vector<std::shared_ptr<TaskMapping>> &finished,
                                     std::vector<std::shared_ptr<TaskMapping>> &pendingReservationQueue) {

    HelperFunctions::processReadyTasks(readyTaskList, applications);
    std::vector<int> order_list;
    if (AlgorithmMode::MODE == algorithm_type::REACTIVE_MOBILE || AlgorithmMode::MODE == algorithm_type::PREALLOCATION ||
        AlgorithmMode::MODE == algorithm_type::PARTITION || AlgorithmMode::MODE == algorithm_type::PROACTIVE) {
        order_list = HelperFunctions::sortReadyTasksByOffload(readyTaskList);
    }

    for (int i = 0; i < readyTaskList.size(); i++) {
        std::shared_ptr<Task> task = (AlgorithmMode::MODE == algorithm_type::REACTIVE_MOBILE || AlgorithmMode::MODE == algorithm_type::PREALLOCATION ||
                                      AlgorithmMode::MODE == algorithm_type::PARTITION || AlgorithmMode::MODE == algorithm_type::PROACTIVE)
                                     ? readyTaskList[order_list[i]]
                                     : readyTaskList[i];

        taskMapping(time, network_graph, inProgress, task, reservationQueue, applications, finished,
                    pendingReservationQueue);
    }
}

void AlgorithmServices::taskMapping(float time, Graph<ComputationNode, std::shared_ptr<EdgeData>> &network_graph,
                                    std::vector<std::shared_ptr<TaskMapping>> &inProgress,
                                    const std::shared_ptr<Task> &selectedTask,
                                    std::vector<std::shared_ptr<TaskMapping>> &reservationQueue,
                                    std::vector<std::shared_ptr<Graph<Task, bool>>> &applications,
                                    std::vector<std::shared_ptr<TaskMapping>> &finished,
                                    std::vector<std::shared_ptr<TaskMapping>> &pendingReservationQueue) {
    std::shared_ptr<TaskMapping> tm;

    int result = NO_RESERVATION_FOUND;

    if (AlgorithmMode::MODE == algorithm_type::PREALLOCATION || AlgorithmMode::MODE == algorithm_type::PARTITION || AlgorithmMode::MODE == algorithm_type::PROACTIVE)
        result = HelperFunctions::IsTaskReserved(reservationQueue, (short) selectedTask->getTotalTaskId());

    if ((AlgorithmMode::MODE == algorithm_type::PREALLOCATION || AlgorithmMode::MODE == algorithm_type::PARTITION ||
         AlgorithmMode::MODE == algorithm_type::PROACTIVE) && result != NO_RESERVATION_FOUND) {
        if (reservationQueue[result]->getProcessingStart() != time)
            return;

        tm = reservationQueue[result];
        tm->setMappingType(mapping_type::in_progress);
        HelperFunctions::removeMappingFromList(reservationQueue, tm);
    } else {
        std::vector<int> parent_ids = applications[selectedTask->getApplicationId()]->getInEdgesID(
                selectedTask->getId());

        tm = std::make_shared<TaskMapping>(TaskMapping((int) parent_ids.size(), selectedTask, mapping_type::in_progress));

        if (AlgorithmMode::MODE == algorithm_type::REACTIVE_MOBILE || AlgorithmMode::MODE == algorithm_type::REACTIVE_BASIC)
            tm->parent_mappings = (std::vector<std::shared_ptr<TaskMapping>>) HelperFunctions::fetchParentMappings(
                    finished, tm->parent_mappings, parent_ids, selectedTask->getApplicationId());

        if (!AlgorithmServices::ChooseNode(network_graph, tm, time + 1,
                                           applications[tm->getTask()->getApplicationId()]->vertices.size()))
            return;

        NetworkTopologyServices::addUploadsToLink(tm, network_graph);
        network_graph.vertices.at(tm->getComputationNodeId())->taskQueue.push_back((tm));

    }
    tm->getTask()->setState(task_state::processing);
    inProgress.push_back(tm);

    if (AlgorithmMode::MODE == algorithm_type::PREALLOCATION)
        PreallocationFunctions::preallocateChildren(time, network_graph, tm, reservationQueue, applications,
                                                    inProgress, pendingReservationQueue);

    /* We only need to run the partition algorithm on the first node */
    if ((AlgorithmMode::MODE == algorithm_type::PARTITION || AlgorithmMode::MODE == algorithm_type::PROACTIVE) && tm->getTask()->getId() == 0)
        PartitionFunctions::proactiveAllocation(time, network_graph, tm, reservationQueue, applications, inProgress,
                                                pendingReservationQueue);
}

bool AlgorithmServices::ChooseNode(Graph<ComputationNode, std::shared_ptr<EdgeData>> &network_graph,
                                   const std::shared_ptr<TaskMapping> &task,
                                   float current_time, unsigned long dag_node_count) {

    auto min_time = (float) INT_MAX;
    bool node_found = false;
    std::unordered_map<int, std::vector<std::shared_ptr<std::pair<float, float>>>> upload_windows;

    for (auto[vertex_id, node]: network_graph.vertices) {
        upload_windows.clear();
        std::pair<float, float> start_and_finish_time = AlgorithmServices::calculateRunTime(task, node, network_graph,
                                                                                            current_time,
                                                                                            upload_windows);

        if (AlgorithmServices::isValidNode(task, start_and_finish_time, node, dag_node_count)) {
            float finish_time = start_and_finish_time.second;

            if ((AlgorithmMode::MODE == algorithm_type::REACTIVE_MOBILE || AlgorithmMode::MODE == algorithm_type::REACTIVE_BASIC) &&
                vertex_id != task->getTask()->getSourceMobileId())
                finish_time = upload_windows.at(RETURN_TO_MOBILE)[0]->second;

            if (finish_time < min_time) {
                node_found = true;
                min_time = start_and_finish_time.second;
                task->setProcessingStart(start_and_finish_time.first);
                task->setProcessingFinish(start_and_finish_time.second);
                task->upload_times = upload_windows;
                task->setComputationNodeId(node->getId());
            }
        }
    }

    if (node_found && !task->upload_times.empty()) {
        //Checking to see if there is at least one time window other than the output
        if (!(task->upload_times.size() == 1 && task->upload_times.count(RETURN_TO_MOBILE))) {
            auto upload_start = (float) INT_MAX;
            float upload_finish = -1;
            for (auto&[key, upload_window_list]: task->upload_times) {
                if (key == RETURN_TO_MOBILE)
                    continue;
                for (const auto &upload_window: upload_window_list) {
                    if (upload_window->first < upload_start)
                        upload_start = upload_window->first;
                    if (upload_window->second > upload_finish)
                        upload_finish = upload_window->second;
                }
            }
            task->setInputUploadStart(upload_start);
            task->setInputUploadFinish(upload_finish);
        } else {
            task->setInputUploadStart(task->getProcessingStart());
            task->setInputUploadFinish(task->getProcessingStart());
        }
        if (task->upload_times.count(RETURN_TO_MOBILE)) {
            auto output_window = task->upload_times.at(RETURN_TO_MOBILE)[0];
            task->setOutputUploadStart(output_window->first);
            task->setOutputUploadFinish(output_window->second);
        }
    }

    return node_found;
}

/*
 * This function returns the process start time
 * and finish time as well as modifying our
 * temp upload list with expected data transfer
 * times for our inputs and outputs
 * */
std::pair<float, float>
AlgorithmServices::calculateRunTime(std::shared_ptr<TaskMapping> task, std::shared_ptr<ComputationNode> node,
                                    Graph<ComputationNode, std::shared_ptr<EdgeData>> &network_graph, float time,
                                    std::unordered_map<int, std::vector<std::shared_ptr<std::pair<float, float>>>> &upload_windows) {

    float rt_local = task->getTask()->getProcessTime(node->getNodeType());
    float ot_up = time;

    upload_windows = AlgorithmServices::calculateParentUploadTimes(time, task, node, network_graph, ot_up);

    if (AlgorithmMode::MODE == algorithm_type::REACTIVE_BASIC || AlgorithmMode::MODE == algorithm_type::REACTIVE_MOBILE)
        upload_windows = AlgorithmServices::calculateOutputReturnTime(time, task, node, network_graph, upload_windows,
                                                                      ot_up + rt_local);

    //Return the process start time and finish time
    return std::make_pair(ot_up, rt_local + ot_up);
}

std::unordered_map<int, std::vector<std::shared_ptr<std::pair<float, float>>>>
AlgorithmServices::calculateParentUploadTimes(float current_time, std::shared_ptr<TaskMapping> tm,
                                              const std::shared_ptr<ComputationNode> &destination_node,
                                              Graph<ComputationNode, std::shared_ptr<EdgeData>> &network_graph,
                                              float &ot_up) {

    HelperFunctions::createTempOccupancyTimes(network_graph);
    std::unordered_map<int, std::vector<std::shared_ptr<std::pair<float, float>>>> upload_windows;

    for (auto &parent : tm->parent_mappings) {
        //If a parent doesn't finish uploading until after the current time, then we cannot begin uploading it.
        float parent_upload_start = (parent->getFinishValue() < current_time) ? current_time
                                                                              : parent->getFinishValue();

        //If it is either of the reactive algorithms, it must upload from the source node every time as they don't use input chaining
        auto source_id = (short) ((AlgorithmMode::MODE == algorithm_type::REACTIVE_BASIC || AlgorithmMode::MODE == algorithm_type::REACTIVE_MOBILE)
                                  ? parent->getTask()->getSourceMobileId() : parent->getComputationNodeId());

        if (source_id != destination_node->getId()) {
            std::shared_ptr<EdgeData> edge = network_graph.out_edges.at(source_id).at(
                    destination_node->getId());

            std::pair<int, std::shared_ptr<std::pair<float, float>>> index_and_time_window = NetworkTopologyServices::findLinkSlot(
                    edge, parent_upload_start,
                    parent->getTask()->getDataOut());

            edge->temp_occupancy_times.insert(edge->temp_occupancy_times.begin() + index_and_time_window.first,
                                              index_and_time_window.second);

            if (!upload_windows.count(source_id))
                upload_windows.insert(
                        {source_id, std::vector<std::shared_ptr<std::pair<float, float>>>()});
            upload_windows.at(source_id).push_back(index_and_time_window.second);

            //If the upload time is greater than our current upload finish
            if (index_and_time_window.second->second > ot_up) ot_up = index_and_time_window.second->second;
        } else {
            if (parent_upload_start > ot_up)
                ot_up = parent_upload_start;
        }
    }

    return upload_windows;
}

std::unordered_map<int, std::vector<std::shared_ptr<std::pair<float, float>>>>
AlgorithmServices::calculateOutputReturnTime(float time, const std::shared_ptr<TaskMapping> &task,
                                             const std::shared_ptr<ComputationNode> &computationNode,
                                             Graph<ComputationNode, std::shared_ptr<EdgeData>> &network_graph,
                                             std::unordered_map<int, std::vector<std::shared_ptr<std::pair<float, float>>>> upload_windows,
                                             float process_finish) {

    //If it's on the source node, no upload needed
    if (computationNode->getId() == task->getTask()->getSourceMobileId())
        return upload_windows;

    //We do not call create temp occupancy times here as it is assumed this will always be called post parent upload windows
    std::shared_ptr<EdgeData> edge = network_graph.out_edges.at(computationNode->getId()).at(
            task->getTask()->getSourceMobileId());

    std::pair<int, std::shared_ptr<std::pair<float, float>>> index_and_time_window = NetworkTopologyServices::findLinkSlot(
            edge, process_finish, task->getTask()->getDataOut());

    if (edge->temp_occupancy_times.empty())
        edge->temp_occupancy_times.push_back(index_and_time_window.second);
    else
        edge->temp_occupancy_times.insert(edge->temp_occupancy_times.begin() + index_and_time_window.first,
                                          index_and_time_window.second);
    if (!upload_windows.count(RETURN_TO_MOBILE))
        upload_windows.insert({RETURN_TO_MOBILE, std::vector<std::shared_ptr<std::pair<float, float>>>()});

    upload_windows.at(RETURN_TO_MOBILE).push_back(index_and_time_window.second);

    return upload_windows;
}

bool AlgorithmServices::isValidNode(std::shared_ptr<TaskMapping> task, std::pair<float, float> time_window,
                                    const std::shared_ptr<ComputationNode> &node, unsigned long dag_node_count) {

    if (AlgorithmMode::MODE == algorithm_type::PARTITION) {
        int split = std::ceil(dag_node_count / 2);
        if (task->getTask()->getId() <= split) {
            if (node->getId() != task->getTask()->getSourceMobileId())
                return false;
        } else if (task->getTask()->getId() > split) {
            if (node->getNodeType() != node_type::cloud && task->getTask()->isOffload()) {
                return false;
            }
        }
    }

    struct ResourceEvent {
        int gpu_usage;
        float ram_usage;
        float storage;

        bool is_increase;
        float time;
    };

    std::vector<ResourceEvent> resource_events;
    resource_events.reserve(node->taskQueue.size() * 2);

    for (const auto &item: node->taskQueue) {
        if (time_window.first <= item->getProcessingFinish() && item->getProcessingStart() <= time_window.second) {
            for (int i = 0; i < 2; i++) {
                resource_events.push_back({1, item->getTask()->getRam(), item->getTask()->getStorage(), (bool) i,
                                           ((bool) i) ? (item->getProcessingStart() < time_window.first)
                                                        ? time_window.first
                                                        : item->getProcessingStart()
                                                      : (item->getProcessingFinish() > time_window.second)
                                                        ? time_window.second
                                                        : item->getProcessingFinish()});
            }
        }
    }

    sort(std::begin(resource_events), std::end(resource_events),
         [](const ResourceEvent &a, const ResourceEvent &b) -> bool {
             if (a.time == b.time)
                 return a.is_increase;
             return a.time < b.time;
         });

    float max_ram_usage = 0;
    float max_storage_usage = 0;
    int max_core_usage = 0;

    float current_ram_usage = 0;
    float current_storage_usage = 0;
    int current_core_usage = 0;

    for (const auto &resource_event: resource_events) {
        if (resource_event.is_increase) {
            current_core_usage += resource_event.gpu_usage;
            current_storage_usage += resource_event.storage;
            current_ram_usage += resource_event.ram_usage;

            if (current_core_usage > max_core_usage)
                max_core_usage = current_core_usage;
            if (current_ram_usage > max_ram_usage)
                max_ram_usage = current_ram_usage;
            if (current_storage_usage > max_storage_usage)
                max_storage_usage = current_storage_usage;
        } else {
            current_core_usage -= resource_event.gpu_usage;
            current_storage_usage -= resource_event.storage;
            current_ram_usage -= resource_event.ram_usage;
        }
    }

    //If the node must be processed locally, we check if this is the source node.
    if (!task->getTask()->isOffload() && node->getNodeType() == node_type::mobile) {
        if (node->getId() != task->getTask()->getSourceMobileId())
            return false;

        else {
            if (!HelperFunctions::checkCapacity((max_ram_usage + task->getTask()->getRam()), max_core_usage + 1,
                                                max_storage_usage + task->getTask()->getStorage(), node))
                return false;
        }
    }

        //If the task can't be offloaded but the node in question is not the mobile
    else if (!task->getTask()->isOffload() && node->getNodeType() != node_type::mobile) {
        return false;
    }

        //Checking if it meets the resource requirements
    else {
        if (!HelperFunctions::checkCapacity((max_ram_usage + task->getTask()->getRam()), max_core_usage + 1,
                                            max_storage_usage + task->getTask()->getStorage(), node))
            return false;
    }

    return true;
}
