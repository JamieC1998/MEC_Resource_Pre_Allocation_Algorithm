//
// Created by Jamie Cotter on 26/01/2022.
//

#include "HelperFunctions.h"
#include "../../Constants/PreallocationConstants.h"
#include <vector>

void HelperFunctions::removeMappingFromList(std::vector<std::shared_ptr<TaskMapping>> &mappingList,
                                            const std::shared_ptr<TaskMapping> &mappingToRemove) {
    int task_id = mappingToRemove->getTask()->getTotalTaskId();
    int index_to_rem = 0;
    for (int i = 0; i < mappingList.size(); i++)
        if (mappingList[i]->getTask()->getTotalTaskId() == task_id) index_to_rem = i;

    mappingList.erase(mappingList.begin() + index_to_rem);
}

int HelperFunctions::IsTaskReserved(std::vector<std::shared_ptr<TaskMapping>> reservationQueue,
                                    short total_task_id) {
    int result = -1;

    for (int i = 0; i < reservationQueue.size(); i++) {
        if (total_task_id == reservationQueue[i]->getTask()->getTotalTaskId()) {
            result = i;
            break;
        }
    }
    return result;
}

void HelperFunctions::processReadyTasks(std::vector<std::shared_ptr<Task>> &readyTaskList,
                                        std::vector<std::shared_ptr<Graph<Task, bool>>> &total_applications) {
    readyTaskList.clear();
    for (auto &application : total_applications) {
        for (const auto &key_task_pair: application->vertices) {
            int key = key_task_pair.first;

            if (key_task_pair.second->getState() != unprocessed && key_task_pair.second->getState() != reserved)
                continue;

            if (!application->in_edges.count(key)) {
                readyTaskList.push_back(key_task_pair.second);
                continue;
            } else if (application->in_edges.count(key)) {
                bool ready = true;

                for (const auto &parentId: application->in_edges.at(key)) {
                    if (application->vertices.at(parentId.first)->getState() != processed) {
                        ready = false;
                        break;
                    }
                }
                if (ready)
                    readyTaskList.push_back(key_task_pair.second);
                else
                    continue;
            }
        }
    }
}

void HelperFunctions::createTempOccupancyTimes(Graph<ComputationNode, std::shared_ptr<EdgeData>> &network) {
    for (auto&[key, outmap]: network.out_edges) {
        for (auto&[ky, out_edge]: outmap) {
            out_edge->clearTempOccupancyTimes();
            out_edge->populateTempOccupancyTimes();
        }
    }
}

bool HelperFunctions::checkCapacity(float ram_usage, int gpu_usage, float storage_usage,
                                    const std::shared_ptr<ComputationNode> &node) {

    if (ram_usage > node->getRam())
        return false;
    if (storage_usage > node->getStorage() * 1024)
        return false;
    if (gpu_usage > node->getGpuCount())
        return false;
    return true;
}

std::vector<int> HelperFunctions::sortReadyTasksByOffload(std::vector<std::shared_ptr<Task>> readyTaskList) {
    std::vector<int> order_list;
    order_list.reserve(readyTaskList.size());

    for (int i = 0; i < readyTaskList.size(); i++) {
        if(!readyTaskList[i].get()->isOffload())
            order_list.push_back(i);
    }
    for (int i = 0; i < readyTaskList.size(); i++) {
        if(readyTaskList[i].get()->isOffload())
            order_list.push_back(i);
    }
    return order_list;
}

std::vector<std::shared_ptr<TaskMapping>>
HelperFunctions::fetchParentMappings(std::vector<std::shared_ptr<TaskMapping>> &task_mappings,
                                     std::vector<std::shared_ptr<TaskMapping>> &parent_mappings,
                                     const std::vector<int> &parent_ids,
                                     int app_id) {

    for (const int &parent_id : parent_ids)
        for (const std::shared_ptr<TaskMapping> &task_mapping: task_mappings)
            if (task_mapping->getTask()->getId() == parent_id && task_mapping->getTask()->getApplicationId() == app_id)
                parent_mappings.push_back(task_mapping);

    return parent_mappings;
}

std::vector<float>
HelperFunctions::generateResourceReleaseTimes(float time, std::vector<std::shared_ptr<TaskMapping>> &in_progress,
                                              std::vector<std::shared_ptr<TaskMapping>> &reservation_queue) {

    std::vector<float> res{};

    for (const auto &item: in_progress)
        if (item->getFinishValue() >= time)
            res.push_back(item->getProcessingFinish());
    for (const auto &item: reservation_queue)
        if (item->getProcessingStart() != RESERVATION_NOT_READY && item->getProcessingFinish() >= time)
            res.push_back(item->getProcessingFinish());

    std::sort(res.begin(), res.end());
    return res;
}

int
HelperFunctions::countApplicationsCompleted(std::vector<std::shared_ptr<Graph<Task, bool>>> &processing_applications) {
    int res = 0;

    for(const auto& application: processing_applications){
        res++;

        for(const auto& task: application->vertices){
            if(task.second->getState() != task_state::processed){
                res--;
                break;
            }
        }
    }
    return res;
}

