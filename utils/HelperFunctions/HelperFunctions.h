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

class HelperFunctions {
public:
    static void removeMappingFromList(std::vector<std::shared_ptr<TaskMapping>> &mappingList,
                                      const std::shared_ptr<TaskMapping> &mappingToRemove);

    static int IsTaskReserved(std::vector<std::shared_ptr<TaskMapping>> reservationQueue,
                              short total_task_id);

    static void processReadyTasks(std::vector<std::shared_ptr<Task>> &readyTaskList,
                        std::vector<std::shared_ptr<Graph<Task, bool>>> &total_applications);

    static void createTempOccupancyTimes(Graph<ComputationNode, std::shared_ptr<EdgeData>>& network);

    static bool checkCapacity(float ram_usage, int gpu_usage, float storage_usage, const std::shared_ptr<ComputationNode>& node);

    static std::vector<int> sortReadyTasksByOffload(std::vector<std::shared_ptr<Task>> readyTaskList);

    static std::vector<std::shared_ptr<TaskMapping>>
    fetchParentMappings(std::vector<std::shared_ptr<TaskMapping>> &task_mappings,
                        std::vector<std::shared_ptr<TaskMapping>> &parent_mappings, const std::vector<int> &parent_ids,
                        int i);

    static std::vector<float>
    generateResourceReleaseTimes(float time, std::vector<std::shared_ptr<TaskMapping>> &in_progress,
                                 std::vector<std::shared_ptr<TaskMapping>> &reservation_queue);

    static int countApplicationsCompleted(std::vector<std::shared_ptr<Graph<Task, bool>>> &processing_applications);
};


#endif //UNTITLED2_HELPERFUNCTIONS_H
