//
// Created by Jamie Cotter on 28/07/2021.
//

#include <Services/NetworkTopologyServices/NetworkTopologyServices.h>
#include <Services/ApplicationTopologyServices/ApplicationTopologyServices.h>
#include <main.h>
#include <thread>
#include "SimulatorFunctions.h"
#include <Models/Mappings/ReservationMapping.h>

using namespace std;
using namespace boost;

/**
 * This function checks if a given node is a viable deployment candidate for a task
 * @param task - The task to be deployed
 * @param vt - The node that is being assessed
 * @return - A boolean value indicating true if it is a valid deployment candidate, false if it isn't
 */
bool SimulatorFunctions::isValidNode(const Task &task, const NetworkVertexData &vt, pair<float, float> timeWindow) {
    ComputationNode node = (vt.type == mobile) ? vt.mobileNode.get() : (vt.type == cloud) ? vt.comp.get()
                                                                                          : vt.edgeNode.get();

    struct ResourceEvent {
        int cpu_usage;
        float ram_usage;
        float storage;

        bool is_increase;
        float time;
    };

    vector<ResourceEvent> resource_events;

    for (const auto &item: node.getReservations()) {
        Task tempTask = item.task;
        if (timeWindow.first <= item.timeWindow.second && item.timeWindow.first <= timeWindow.second) {
            for (int i = 0; i < 2; i++) {
                resource_events.push_back({
                                                  tempTask.getCoreCount(),
                                                  tempTask.getRam(),
                                                  tempTask.getStorage(),
                                                  (bool) i,
                                                  ((bool) i) ? (item.timeWindow.first < timeWindow.first)
                                                               ? timeWindow.first : item.timeWindow.first
                                                             : (item.timeWindow.second > timeWindow.second)
                                                               ? timeWindow.second : item.timeWindow.second
                                          });
            }
        }
    }

    for (const auto &item: node.getTaskVector()) {
        Task tempTask = item.task;
        if (timeWindow.first <= item.timeWindow.second && item.timeWindow.first <= timeWindow.second) {
            for (int i = 0; i < 2; i++) {
                resource_events.push_back({
                                                  tempTask.getCoreCount(),
                                                  tempTask.getRam(),
                                                  tempTask.getStorage(),
                                                  (bool) i,
                                                  ((bool) i) ? (item.timeWindow.first < timeWindow.first)
                                                               ? timeWindow.first : item.timeWindow.first
                                                             : (item.timeWindow.second > timeWindow.second)
                                                               ? timeWindow.second : item.timeWindow.second
                                          });
            }
        }
    }

    sort(std::begin(resource_events), std::end(resource_events),
         [](const ResourceEvent &a, const ResourceEvent &b) -> bool {
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
            current_core_usage += resource_event.cpu_usage;
            current_storage_usage += resource_event.storage;
            current_ram_usage += resource_event.ram_usage;

            if (current_core_usage > max_core_usage)
                max_core_usage = current_core_usage;
            if (current_ram_usage > max_ram_usage)
                max_ram_usage = current_ram_usage;
            if (current_storage_usage > max_storage_usage)
                max_storage_usage = current_storage_usage;
        } else {
            current_core_usage -= resource_event.cpu_usage;
            current_storage_usage -= resource_event.storage;
            current_ram_usage -= resource_event.ram_usage;
        }
    }

    //If the node must be processed locally, we check if this is the source node.
    if (!task.isOffload() && vt.type == mobile) {
        if (node.getId() != task.getSourceMobileId())
            return false;

        else {
            if ((max_ram_usage + task.getRam()) > node.getRam())
                return false;
            if ((max_storage_usage + task.getStorage()) > node.getStorage())
                return false;
            if ((max_core_usage + task.getCoreCount()) > node.getCores())
                return false;
        }
    }

        //If the task can't be offloaded but the node in question is not the mobile
    else if (!task.isOffload() && vt.type != mobile) {
        return false;
    }

        //Checking if it meets the resource requirements
    else {
        if ((max_ram_usage + task.getRam()) > node.getRam())
            return false;
        if ((max_storage_usage + task.getStorage()) > node.getStorage())
            return false;
        if ((max_core_usage + task.getCoreCount()) > node.getCores())
            return false;
    }
    return true;
}

/**
 * This function iterates through list of nodes
 * retrieves the run time for a task on each node
 * selects the node with the minimum run time.
 *
 * @param networkList - List of all the vertices in our topology
 * @param task - Task that is to be offloaded
 * @param current_time - The current time in the simulation
 * @param topology -  The graph representing the network
 * @return - Returns the index of the node with the lowest run time and the estimated time of completion
 */
std::pair<int, float> SimulatorFunctions::ChooseNode(
        std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> &networkList,
        Task &task, float current_time, NetworkTopology &topology, const std::vector<TaskMapping> &parents,
        float &startTime, std::vector<pair<float, float>> &finish_times_list,
        std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> &map) {

    int index = -1;
    float min_run_time = ((float) INT_MAX);

    int source_node_index = 0;

    //Get the graph index of the source device of the task
    for (auto vertex = networkList.begin(); vertex != networkList.end(); vertex++) {
        ComputationNode temp = (vertex->m_property.type == mobile) ? vertex->m_property.mobileNode.get()
                                                                   : (vertex->m_property.type == cloud)
                                                                     ? vertex->m_property.comp.get()
                                                                     : vertex->m_property.edgeNode.get();

        if (temp.getId() == task.getSourceMobileId()) {
            source_node_index = ((int) std::distance(networkList.begin(), vertex));
            break;
        }
    }

    vector<pair<float, float>> tmp_link_finish_times;

    for (auto vertex = networkList.begin(); vertex != networkList.end(); vertex++) {
        int current_node_index = ((int) std::distance(networkList.begin(), vertex));
        float tempStartTime = 0.0f;
        tmp_link_finish_times.clear();
        float current_run_time = SimulatorFunctions::calculateRunTime(task, current_node_index,
                                                                      networkList, current_time, topology, parents,
                                                                      tempStartTime, tmp_link_finish_times, map);

        std::pair<float, float> applicationTimeRange = make_pair(tempStartTime, current_run_time);

        if (SimulatorFunctions::isValidNode(task, vertex->m_property, applicationTimeRange)) {
            if (current_run_time < min_run_time) {
                index = current_node_index;
                min_run_time = current_run_time;
                startTime = tempStartTime;
                finish_times_list.clear();
                std::copy(std::begin(tmp_link_finish_times), std::end(tmp_link_finish_times), std::back_inserter(finish_times_list));
            }
        }
    }

    return make_pair(index, min_run_time);
}

float SimulatorFunctions::calculateRunTime(Task &task, int currentNodeIndex,
                                           std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> &networkList,
                                           float current_time, NetworkTopology &network,
                                           std::vector<TaskMapping> parents,
                                           float &startTime, std::vector<pair<float, float>> &tmp_finish_times,
                                           std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> &map) {

    ComputationNode current_node = (networkList[currentNodeIndex].m_property.type == mobile)
                                   ? networkList[currentNodeIndex].m_property.mobileNode.get()
                                   : (networkList[currentNodeIndex].m_property.type == cloud)
                                     ? networkList[currentNodeIndex].m_property.comp.get()
                                     : networkList[currentNodeIndex].m_property.edgeNode.get();


    float rt_local = (float) task.getMillionsOfInstructions() / (float) current_node.getMillionsInstructionsPerCore();

    float ot_up = 0.0f;

    //If the task is not allowed to be offloaded and this is the source task we do not need to calculate bandwidth
    //as there's no data to transfer
    ot_up += current_time;

    for (auto & parent : parents) {
        float parent_upload_start = (parent.absoluteFinish + 0.00001f < current_time) ? current_time : parent.absoluteFinish + 0.00001f;
        pair<float, float> time_window = make_pair(-1, parent_upload_start);
        if(parent.nodeIndex != currentNodeIndex){
            EdgePropertyData edge = (parent.nodeIndex < currentNodeIndex) ? map.at(parent.nodeIndex).at(currentNodeIndex) : map.at(currentNodeIndex).at(parent.nodeIndex);
            float bw = INT_MAX - edge.edge_weight;
            time_window = NetworkTopologyServices::findLinkSlot(edge.occupancy_times, parent_upload_start, parent.task.get().task->getDataOut(), bw);
        }

        tmp_finish_times.push_back(time_window);
        if (time_window.second > ot_up) ot_up = time_window.second;
    }

    startTime = ot_up;

    return rt_local + ot_up;
}

vector<pair<int, bool>> SimulatorFunctions::processReadyTasks(
        vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex *> &readyTaskList,
        vector<vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> *total_task_lists) {
    //Emptying the task list
    readyTaskList.clear();

    //Iterating through each application queue
    for (auto &taskList: *total_task_lists) {
        auto tempRes = SimulatorFunctions::getReadyTasks(taskList);
        readyTaskList.insert(std::end(readyTaskList), std::begin(tempRes), std::end(tempRes));
    }


    vector<pair<int, bool>> tvd;
    tvd.reserve(readyTaskList.size());

    for (int i = 0; i < readyTaskList.size(); i++) {
        tvd.emplace_back(i, readyTaskList[i]->m_property.task.get().isOffload());
    }

    std::sort(tvd.begin(), tvd.end(), [](pair<int, bool> a, pair<int, bool> b) {
        return a.second > b.second;
    });

    return tvd;
}

int
SimulatorFunctions::checkIfTaskReserved(vector<ReservationMapping> &reservationQueue, TaskVertexData &selectedTask) {
    int index = -1;

    for (int i = 0; i < reservationQueue.size(); i++) {
        if (selectedTask.task.get().getId() == reservationQueue[i].task->task->getId()) {
            index = i;
            break;
        }
    }
    return index;
}

void SimulatorFunctions::taskMapping(float time, NetworkTopology &network, std::vector<TaskMapping> *inProgress,
                                     TaskVertexData &selectedTask,
                                     std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> &networkVertexList,
                                     std::vector<ReservationMapping> &reservationQueue,
                                     std::vector<std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> &total_task_lists,
                                     std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> &hash_map) {

    int result = SimulatorFunctions::checkIfTaskReserved(reservationQueue, selectedTask);
    int nodeIndex = -1;

    float estimatedFinishTime = -1.0f;
    float startTime = 0;

    NetworkVertexData *selectedNode;
    vector<TaskMapping> parents;
    //If the task is pre-allocated
    if (result != -1) {
        auto &reservedTask = reservationQueue[result];

        // If our reservation isn't ready
        if (reservedTask.startTime != time)
            return;

        nodeIndex = reservationQueue[result].nodeIndex;
        selectedNode = &networkVertexList[(int) nodeIndex].m_property;
        estimatedFinishTime = reservedTask.finishTime;
        startTime = reservedTask.startTime;

        //Upcasting our selected node to base class for convenience
        ComputationNode &node = (networkVertexList[nodeIndex].m_property.type == mobile)
                                ? networkVertexList[nodeIndex].m_property.mobileNode.get()
                                : (networkVertexList[nodeIndex].m_property.type ==
                                   cloud)
                                  ? networkVertexList[nodeIndex].m_property.comp.get()
                                  : networkVertexList[nodeIndex].m_property.edgeNode.get();


        int index = -1;
        auto &reservations = const_cast<vector<NodeMapping> &>(node.getReservations());
        for (int i = 0; i < reservations.size(); i++) {
            if (reservations[i].task.getId() == reservedTask.task->task->getId()) {
                index = i;
                break;
            }
        }

        reservations.erase(std::begin(reservations) + index);
        reservationQueue.erase(std::begin(reservationQueue) + result);
    } else {
        vector<pair<float, float>> finish_times;
        pair<int, float> selectedNodeData = SimulatorFunctions::ChooseNode(networkVertexList,
                                                                           selectedTask.task.get(), time,
                                                                           network, parents, startTime, finish_times,
                                                                           hash_map);

        //If a valid node has not been found
        if (selectedNodeData.first == -1)
            return;

        estimatedFinishTime = selectedNodeData.second;
        selectedNode = &networkVertexList[selectedNodeData.first].m_property;
        nodeIndex = selectedNodeData.first;

        NetworkTopologyServices::addUploadsToLink(make_pair(finish_times, parents), selectedNodeData.first,
                                                  network, hash_map);
    }

    selectedTask.task.get().setInProgress(true);

    //Upcasting our selected node to base class for convenience
    ComputationNode &node = (selectedNode->type == mobile) ? selectedNode->mobileNode.get()
                                                           : (selectedNode->type ==
                                                              cloud)
                                                             ? selectedNode->comp.get()
                                                             : selectedNode->edgeNode.get();



    //Adding the selected task to our selected node
    (const_cast<vector<struct NodeMapping> &>(node.getTaskVector())).push_back(
            {selectedTask.task.get(), make_pair(startTime, estimatedFinishTime)});
    TaskMapping map{startTime, estimatedFinishTime, selectedTask, selectedNode, nodeIndex};
    inProgress->push_back(map);

    SimulatorFunctions::preallocateTasks(total_task_lists, selectedTask, networkVertexList, reservationQueue, network,
                                         time, map, inProgress, hash_map);
}

void SimulatorFunctions::preallocateTasks(
        std::vector<std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> &total_task_lists,
        TaskVertexData &selectedTask,
        std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> &networkVertexList,
        std::vector<ReservationMapping> &reservationQueue, NetworkTopology &network, float time, TaskMapping map,
        std::vector<TaskMapping> *inProgress,
        std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> &unorderedMap) {

    auto outGoingTasks = SimulatorFunctions::getOutGoingTasks(total_task_lists, selectedTask);

    std::sort(std::begin(outGoingTasks), std::end(outGoingTasks), [](
            detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex *a,
            detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex *b) {
        return a->m_property.task->isOffload() > b->m_property.task->isOffload();
    });

    for (auto &outGoingTask : outGoingTasks) {
        int index = -1;
        for (int i = 0; i < reservationQueue.size(); i++) {
            if (reservationQueue[i].task->task->getId() == outGoingTask->m_property.task->getId()) {
                index = i;
            }
        }
        if (index != -1) {
            reservationQueue[index].parents.push_back(map);
        } else {
            std::vector<TaskMapping> parents;
            parents.push_back(map);


            ReservationMapping reservedNode{&outGoingTask->m_property, -1, -1, -1, parents,
                                            static_cast<int>(outGoingTask->m_in_edges.size())};

            //1st index is the node it is allocated to, second is the time it will take, third is the task, 4th is placeholder
            reservationQueue.push_back(reservedNode);
            index = reservationQueue.size() - 1;
        }
        if (reservationQueue[index].parents.size() == reservationQueue[index].parentCount) {
            float startTime = 0.0f;
            vector<float> finish_times;
            vector<pair<float, float>> parent_result_upload_windows;
            auto chooseNode = SimulatorFunctions::ChooseNode(networkVertexList, outGoingTask->m_property.task.get(),
                                                             map.absoluteStart,
                                                             network, reservationQueue[index].parents,
                                                             startTime, parent_result_upload_windows,
                                                             unorderedMap);

            //If we cannot preallocate a node for when our last task in our parents ends
            //Then we must attempt preallocate, fast forwarding through the state of the network till
            //a node can be preallocated.
            if (chooseNode.first == -1) {
                finish_times.clear();
                float parent_start_time = reservationQueue[index].parents[reservationQueue[index].parents.size() -
                                                                          1].absoluteStart;
                for (const auto &item: *inProgress) {
                    if (parent_start_time <= item.absoluteFinish)
                        finish_times.push_back(item.absoluteFinish + 0.0001f);
                }

                for (const auto &item: reservationQueue) {
                    if (parent_start_time <= item.finishTime && item.finishTime != -1)
                        finish_times.push_back(item.finishTime + 0.0001f);
                }

                std::sort(finish_times.begin(), finish_times.end());

                for (const float f_time: finish_times) {
                    startTime = 0.0f;
                    parent_result_upload_windows.clear();
                    chooseNode = SimulatorFunctions::ChooseNode(networkVertexList,
                                                                outGoingTask->m_property.task.get(),
                                                                f_time,
                                                                network, reservationQueue[index].parents,
                                                                startTime, parent_result_upload_windows,
                                                                unorderedMap);
                    if (chooseNode.first != -1)
                        break;
                }
            }

            ComputationNode &node = (networkVertexList[chooseNode.first].m_property.type == mobile)
                                    ? networkVertexList[chooseNode.first].m_property.mobileNode.get()
                                    : (networkVertexList[chooseNode.first].m_property.type ==
                                       cloud)
                                      ? networkVertexList[chooseNode.first].m_property.comp.get()
                                      : networkVertexList[chooseNode.first].m_property.edgeNode.get();

            reservationQueue[index].startTime = startTime;
            reservationQueue[index].finishTime = chooseNode.second;
            reservationQueue[index].nodeIndex = chooseNode.first;


            vector<NodeMapping> nodeReservations = node.getReservations();
            nodeReservations.push_back(
                    {reservationQueue[index].task->task.get(), make_pair(startTime, chooseNode.second)});
            node.setReservations(nodeReservations);

            NetworkTopologyServices::addUploadsToLink(make_pair(parent_result_upload_windows, reservationQueue[index].parents),
                                                      chooseNode.first, network, unorderedMap);
        }
    }
}

vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex *>
SimulatorFunctions::getOutGoingTasks(
        vector<vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
        &total_task_lists,
        TaskVertexData &selectedTask) {
    vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex *> res;

    for (auto &total_task_list : total_task_lists) {
        for (auto &task_item : total_task_list) {
            if (task_item.m_property.task.get().getId() == selectedTask.task->getId()) {
                for (auto &m_out_edge : task_item.m_out_edges)
                    res.push_back(&total_task_list[m_out_edge.m_target]);
                break;
            }
        }
    }

    return res;
}

void SimulatorFunctions::runAlgorithm(
        std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex *> &readyTaskList,
        std::vector<std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> &total_task_lists,
        std::vector<TaskMapping> &inProgress,
        std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> &networkVertexList,
        NetworkTopology &network, float time, std::vector<ReservationMapping> &reservationQueue,
        std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> &map) {

    vector<pair<int, bool>> order = SimulatorFunctions::processReadyTasks(readyTaskList, &total_task_lists);

    int task_count = ((int) order.size());

    for (int i = 0; i < task_count; i++) {
        TaskVertexData &selectedTask = readyTaskList[order[i].first]->m_property;

        SimulatorFunctions::taskMapping(time, network, &inProgress, selectedTask, networkVertexList, reservationQueue,
                                        total_task_lists, map);
    }
}

vector<ReservationMapping> SimulatorFunctions::sortReservations(vector<ReservationMapping> reservationMappings) {
    sort(std::begin(reservationMappings), std::end(reservationMappings),
         [](ReservationMapping taskMapA, ReservationMapping taskMapB) {
             return taskMapA.startTime > taskMapB.startTime;
         });
    return reservationMappings;
}

void SimulatorFunctions::programLoop(NetworkTopology &network, vector<ApplicationEvent> incoming_applications,
                                     float completion_time, char *output_file_name, char *lower_bound_file_name) {
    int total_task_count = 0;

    for (auto &incoming_application : incoming_applications)
        total_task_count += ((int) incoming_application.application.m_vertices.size());

    //Represents time in seconds
    float time = 0.0f;

    //Will contain queue of tasks for each application currently offloading
    vector<vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> total_task_lists;

    //Retrieving the list of vertices and their edges(in & out) in both our generated application and our network
    auto networkVertexList = NetworkTopologyServices::getVertices(network);
    int super_node_MI = NetworkTopologyServices::getSuperNodeMI(networkVertexList);

    //Will hold the tasks ready to be offloaded in each loop
    vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex *>
            readyTaskList;

    //Contains a list of tasks mapped to nodes currently computing
    vector<TaskMapping> inProgress;

    //Contains a finished list of tasks
    vector<TaskMapping> finished;

    vector<ReservationMapping> reservationQueue;

    vector<float> lower_bound_application_times;

    std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> edge_data = NetworkTopologyServices::generateEdgeMap(network, networkVertexList.size());

    while (!(time >= completion_time || (finished.size() == total_task_count && incoming_applications.empty()))) {
        //Sorting the current event list from most recent to oldest time
        inProgress = SimulatorFunctions::sortEventList(inProgress);
        reservationQueue = SimulatorFunctions::sortReservations(reservationQueue);

        //Moving to the next event
        SimulatorFunctions::UpdateEventList(inProgress, finished, time, reservationQueue, completion_time,
                                            &incoming_applications);

        SimulatorFunctions::checkIncomingApplications(&total_task_lists, &incoming_applications, time,
                                                      lower_bound_application_times, super_node_MI);

        SimulatorFunctions::runAlgorithm(readyTaskList, total_task_lists, inProgress, networkVertexList, network, time,
                                         reservationQueue, edge_data);
    }

    main::logResults(finished, output_file_name);
    main::logLowerBoundTimes(lower_bound_application_times, lower_bound_file_name);
}

/**
 * This function is responsible for checking the incoming application queue in order to decide
 * when to begin offloading an application
 *
 * @param total_task_list - A matrix with each top level index containing an applications list of tasks
 * @param applications - The applications yet to be offloaded
 * @param current_time - The current time of the simulation
 */
void SimulatorFunctions::checkIncomingApplications(
        vector<vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> *total_task_list,
        vector<ApplicationEvent> *applications,
        float current_time, vector<float> &lower_bound_time,
        int super_node_MI) {

    vector<ApplicationEvent> &incoming_application = *applications;
    for (int i = 0; i < incoming_application.size(); i++) {
        if (incoming_application[i].ready_time <= current_time) {
            auto application_tasks = ApplicationTopologyServices::getVertices(incoming_application[i].application);

            lower_bound_time.push_back(ApplicationTopologyServices::calculateLowerBound(application_tasks, 10));
            total_task_list->push_back(application_tasks);
            incoming_application.erase(incoming_application.begin() + i);
            i = i - 1;
        }
    }
}

/**
 * This function processes the next event to complete in our event list
 * freeing up the nodes associated so that they can re-enter the ready node pool
 * Adding the result of the event to finished queue
 *
 * @param inProgress - A reference to a vector of task mappings in progress
 * @param finished - A reference to a vector that contains task mappings that are completed
 * @param time - A reference to the current time of the application to be updated with the current event finish time
 */
void SimulatorFunctions::UpdateEventList(vector<TaskMapping> &inProgress, vector<TaskMapping> &finished, float &time,
                                         vector<ReservationMapping> reservationMapping, float completion_time,
                                         vector<ApplicationEvent> *applications) {
    vector<ApplicationEvent> &incoming_applications = *applications;
    /**
         * If there are no tasks in progress and no tasks ready, that means that
         * we are waiting for the next application to enter
         */
    //        if (inProgress.empty() && readyTaskList.empty() && !incoming_applications.empty()) {
    auto app_min_time = (float) INT_MAX;
    bool app_chosen = false;
    if (!incoming_applications.empty()) {
        for (auto &incoming_application : incoming_applications) {
            if (incoming_application.ready_time < app_min_time) {
                app_min_time = incoming_application.ready_time;
                app_chosen = true;
            }
        }
    }

    vector<ReservationMapping> sortedReservationMappings;
    std::copy_if(reservationMapping.begin(),
                 reservationMapping.end(),
                 std::back_inserter(sortedReservationMappings),
                 [](const ReservationMapping &mapping) {
                     return mapping.startTime != -1;
                 });

    if (((!inProgress.empty() && !sortedReservationMappings.empty()) &&
         sortedReservationMappings.back().startTime <= inProgress.back().absoluteFinish)
        || (!sortedReservationMappings.empty() && inProgress.empty())) {
        float finish_time = sortedReservationMappings.back().startTime;
        if (app_chosen && app_min_time < finish_time) time = app_min_time;
        else time = finish_time;

    } else if (!inProgress.empty()) {
        TaskMapping tM = inProgress.back();
        if (app_chosen && app_min_time < tM.absoluteFinish) {
            if (app_min_time > completion_time) time = completion_time;
            else time = app_min_time;
            return;
        }
        if (tM.absoluteFinish > completion_time) {
            time = completion_time;
            return;
        }
        inProgress.pop_back();

        time = tM.absoluteFinish;
        tM.task.get().task.get().setDone(true);
        tM.task.get().task.get().setInProgress(false);

        ComputationNode &node = (tM.node->type == mobile) ? tM.node->mobileNode.get() : (tM.node->type ==
                                                                                         cloud)
                                                                                        ? tM.node->comp.get()
                                                                                        : tM.node->edgeNode.get();

        int task_index_to_rem = -1;
        for (int i = 0; i < node.getTaskVector().size(); i++) {
            if (tM.task.get().task->getId() == node.getTaskVector()[i].task.getId()) {
                task_index_to_rem = i;
                break;
            }
        }

        (const_cast<vector<struct NodeMapping> &> (node.getTaskVector())).erase(
                node.getTaskVector().begin() + task_index_to_rem);

        finished.push_back(tM);
    } else if (app_chosen) {
        time = app_min_time;
    }
}

vector<TaskMapping> SimulatorFunctions::sortEventList(vector<TaskMapping> eventList) {
    sort(std::begin(eventList), std::end(eventList),
         [](TaskMapping taskMapA, TaskMapping taskMapB) {
             return taskMapA.absoluteFinish > taskMapB.absoluteFinish;
         });

    return eventList;
}

/**
 * @param taskList - The list of tasks that represent all the vertices in an application
 * @return - A vector containing only the tasks ready to be offloaded
 */
vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex *>
SimulatorFunctions::getReadyTasks(
        vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> &taskList) {

    vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex *> res;
    for (auto &vertex : taskList) {
        Task &tempTask = vertex.m_property.task.get();
        if (!vertex.m_property.task->isDone() && !vertex.m_property.task->isInProgress()) {
            //If a task depends on no other tasks and is not in progress or completed
            // then it is ready to be offloaded
            if (vertex.m_in_edges.empty()) {
                res.emplace_back(&vertex);
                continue;
            } else {
                bool ready = true;
                //Checking to see if the incoming edges(tasks a task depends upon) are completed
                for (auto edge : vertex.m_in_edges) {
                    Task &edgeTemp = taskList[edge.m_target].m_property.task.get();
                    if (!taskList[edge.m_target].m_property.task->isDone() ||
                        taskList[edge.m_target].m_property.task->isInProgress()) {
                        ready = false;
                    }
                }
                if (ready)
                    res.emplace_back(&vertex);
            }
        }
    }

    return res;
}
