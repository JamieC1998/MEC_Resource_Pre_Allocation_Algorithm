//
// Created by Jamie Cotter on 07/02/2022.
//

#include "PartitionFunctions.h"
#include "../PreallocationFunctions/PreallocationFunctions.h"
#include "../../utils/HelperFunctions/HelperFunctions.h"

void PartitionFunctions::proactiveAllocation(Graph<ComputationNode, std::shared_ptr<EdgeData>> &network,
                                             std::shared_ptr<TaskMapping> &rootMapping,
                                             std::vector<std::shared_ptr<TaskMapping>> &reservationQueue,
                                             std::vector<std::shared_ptr<Graph<Task, bool>>> &applications,
                                             std::vector<std::shared_ptr<TaskMapping>> &inProgress,
                                             std::vector<std::shared_ptr<TaskMapping>> &pendingReservationsQueue) {

    PartitionFunctions::partitionAllocateReadyTasks(rootMapping, applications, network, inProgress, reservationQueue,
                                                    pendingReservationsQueue, -1);
    int reserved_count = 1;
    while (reserved_count < applications[rootMapping->getTask()->getApplicationId()]->vertices.size()) {
        std::vector<int> readyMappings = PartitionFunctions::generateReadyReservations(
                pendingReservationsQueue);

        /* This is done as when the preallocateChild functions in Pre-allocationFunctions
         * finishes pre-allocating a child, it removes it from the pendingReservationQueue,
         * however, in this case as we are pulling pendingReservations from the pendingReservationsQueue
         * by index, it will lead to an index out of bounds error */

        for (const int &ready_mapping_index: readyMappings) {
            std::shared_ptr<TaskMapping> parent = pendingReservationsQueue[ready_mapping_index];
            PartitionFunctions::partitionAllocateReadyTasks(parent, applications, network, inProgress, reservationQueue,
                                                            pendingReservationsQueue, ready_mapping_index);
            reserved_count++;
        }

        std::sort(readyMappings.begin(), readyMappings.end());
        for (int i = 0; i < readyMappings.size(); i++)
            pendingReservationsQueue.erase(pendingReservationsQueue.begin() + readyMappings[i] - i);
    }
}


std::vector<int>
PartitionFunctions::generateReadyReservations(std::vector<std::shared_ptr<TaskMapping>> &pendingReservations) {
    std::vector<int> res;

    std::sort(pendingReservations.begin(), pendingReservations.end(),
              [](const std::shared_ptr<TaskMapping> &a, const std::shared_ptr<TaskMapping> &b) -> bool {
                  return a->getInputUploadStart() < b->getInputUploadStart();
              });
    for (int i = 0; i < pendingReservations.size(); i++) {
        if (pendingReservations[i]->getTask()->isOffload() &&
            pendingReservations[i]->getTotalParentCount() == pendingReservations[i]->parent_mappings.size())
            res.push_back(i);
    }
    for (int i = 0; i < pendingReservations.size(); i++) {
        if (!pendingReservations[i]->getTask()->isOffload() &&
            pendingReservations[i]->getTotalParentCount() == pendingReservations[i]->parent_mappings.size())
            res.push_back(i);
    }
    return res;
}

void PartitionFunctions::partitionAllocateReadyTasks(std::shared_ptr<TaskMapping> &parent,
                                                     std::vector<std::shared_ptr<Graph<Task, bool>>> &applications,
                                                     Graph<ComputationNode, std::shared_ptr<EdgeData>> &network,
                                                     std::vector<std::shared_ptr<TaskMapping>> &inProgress,
                                                     std::vector<std::shared_ptr<TaskMapping>> &reservationQueue,
                                                     std::vector<std::shared_ptr<TaskMapping>> &pendingReservationsQueue,
                                                     int ready_mapping_index) {

    /* If ready mapping index is -1, it means that we are partitioning the root node which has already
    * been allocated in AlgorithmFunctions::taskMapping and thus does not exist in the pendingReservations
    * Queue. */
    if (ready_mapping_index != -1)
        PreallocationFunctions::preallocateChild(ready_mapping_index,
                                                 parent->getInputUploadStart(),
                                                 network,
                                                 inProgress,
                                                 reservationQueue,
                                                 pendingReservationsQueue,
                                                 applications[parent->getTask()->getApplicationId()]->vertices.size());

    std::vector<std::shared_ptr<Task>> children_tasks = applications[parent->getTask()->getApplicationId()]->getOutEdgesVertex(
            parent->getTask()->getId());

    HelperFunctions::sortReadyTasksByOffload(children_tasks);
    for (auto &child: children_tasks)
        PreallocationFunctions::createUpdateChildReservation(
                parent, applications, child, pendingReservationsQueue);
}


