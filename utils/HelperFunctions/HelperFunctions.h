//
// Created by Jamie Cotter on 26/01/2022.
//

#ifndef UNTITLED2_HELPERFUNCTIONS_H
#define UNTITLED2_HELPERFUNCTIONS_H

#include <vector>
#include <memory>
#include "../../Model/TaskMapping/TaskMapping.h"
#include "../../Graph/Graph.h"
#include "../../Model/ComputationNode/ComputationNode.h"
#include "../../Graph/EdgeData/EdgeData.h"

namespace HelperFunctions {
    void removeMappingFromList(std::vector<std::shared_ptr<TaskMapping>> &mappingList,
                                      const std::shared_ptr<TaskMapping> &mappingToRemove);

    int IsTaskReserved(std::vector<std::shared_ptr<TaskMapping>> reservationQueue,
                              short total_task_id);

    void processReadyTasks(std::vector<std::shared_ptr<Task>> &readyTaskList,
                        std::vector<std::shared_ptr<Graph<Task, bool>>> &total_applications);

    void createTempOccupancyTimes(Graph<ComputationNode, std::shared_ptr<EdgeData>>& network);

    bool checkCapacity(float ram_usage, int gpu_usage, float storage_usage, const std::shared_ptr<ComputationNode>& node);

    std::vector<int> sortReadyTasksByOffload(std::vector<std::shared_ptr<Task>> readyTaskList);

    std::vector<std::shared_ptr<TaskMapping>>
    fetchParentMappings(std::vector<std::shared_ptr<TaskMapping>> &task_mappings,
                        std::vector<std::shared_ptr<TaskMapping>> &parent_mappings, const std::vector<int> &parent_ids,
                        int i);

    std::vector<float>
    generateResourceReleaseTimes(float time, std::vector<std::shared_ptr<TaskMapping>> &in_progress,
                                 std::vector<std::shared_ptr<TaskMapping>> &reservation_queue);

    int countApplicationsCompleted(std::vector<std::shared_ptr<Graph<Task, bool>>> &processing_applications);
};


#endif //UNTITLED2_HELPERFUNCTIONS_H
