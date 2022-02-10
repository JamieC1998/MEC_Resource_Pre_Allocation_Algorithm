//
// Created by Jamie Cotter on 07/02/2022.
//

#ifndef UNTITLED2_PARTITIONFUNCTIONS_H
#define UNTITLED2_PARTITIONFUNCTIONS_H


#include "../../Graph/Graph.h"
#include "../../Model/ComputationNode/ComputationNode.h"
#include "../../Graph/EdgeData/EdgeData.h"
#include "../../Model/TaskMapping/TaskMapping.h"
#include "../../Model/Task/Task.h"

class PartitionFunctions {
public:

    static void proactiveAllocation(float time, Graph<ComputationNode, std::shared_ptr<EdgeData>> &network,
                                    std::shared_ptr<TaskMapping> &rootMapping,
                                    std::vector<std::shared_ptr<TaskMapping>> &reservationQueue,
                                    std::vector<std::shared_ptr<Graph<Task, bool>>> &applications,
                                    std::vector<std::shared_ptr<TaskMapping>> &inProgress,
                                    std::vector<std::shared_ptr<TaskMapping>> &pendingReservationsQueue);

    static std::vector<int>
    generateReadyReservations(std::vector<std::shared_ptr<TaskMapping>> &pendingReservationQueue);

    static void partitionAllocateReadyTasks(std::shared_ptr<TaskMapping> &parent,
                                            std::vector<std::shared_ptr<Graph<Task, bool>>> &applications,
                                            Graph<ComputationNode, std::shared_ptr<EdgeData>> &network,
                                            std::vector<std::shared_ptr<TaskMapping>> &inProgress,
                                            std::vector<std::shared_ptr<TaskMapping>> &reservationQueue,
                                            std::vector<std::shared_ptr<TaskMapping>> &pendingReservationsQueue,
                                            int ready_mapping_index);
};


#endif //UNTITLED2_PARTITIONFUNCTIONS_H
