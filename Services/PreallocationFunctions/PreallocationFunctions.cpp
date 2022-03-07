//
// Created by Jamie Cotter on 05/02/2022.
//

#include "PreallocationFunctions.h"
#include "../../utils/HelperFunctions/HelperFunctions.h"
#include "../../Constants/GeneralConstants.h"
#include "../AlgorithmServices/AlgorithmServices.h"
#include "../NetworkTopologyServices/NetworkTopologyServices.h"
#include "../../Constants/AlgorithmFlag/AlgorithmFlag.h"
#include <iostream>

void PreallocationFunctions::preallocateChildren(float time, Graph<ComputationNode, std::shared_ptr<EdgeData>> &network,
                                                 std::shared_ptr<TaskMapping> &parent_mapping,
                                                 std::vector<std::shared_ptr<TaskMapping>> &reservation_queue,
                                                 std::vector<std::shared_ptr<Graph<Task, bool>>> &application_list,
                                                 std::vector<std::shared_ptr<TaskMapping>> &inProgressList,
                                                 std::vector<std::shared_ptr<TaskMapping>> &pendingReservationList) {
    std::vector<std::shared_ptr<Task>> children_tasks = application_list[parent_mapping->getTask()->getApplicationId()]->getOutEdgesVertex(
            parent_mapping->getTask()->getId());

    HelperFunctions::sortReadyTasksByOffload(children_tasks);

    for (auto &child: children_tasks) {
        int reserved_child_index = PreallocationFunctions::createUpdateChildReservation(
                parent_mapping, application_list, child, pendingReservationList);

        std::shared_ptr<TaskMapping> reservedChild = pendingReservationList[reserved_child_index];
        if (reservedChild->parent_mappings.size() == reservedChild->getTotalParentCount()) {
            PreallocationFunctions::preallocateChild(reserved_child_index, parent_mapping->getFinishValue(),
                                                     network,
                                                     inProgressList, reservation_queue, pendingReservationList,
                                                     application_list[reservedChild->getTask()->getApplicationId()]->vertices.size());
        }
    }
}

int PreallocationFunctions::createUpdateChildReservation(
        std::shared_ptr<TaskMapping> &parent_mapping,
        std::vector<std::shared_ptr<Graph<Task, bool>>> &application_list,
        std::shared_ptr<Task> &child,
        std::vector<std::shared_ptr<TaskMapping>> &pendingReservationList) {

    std::shared_ptr<TaskMapping> reservedChild;

    int result = HelperFunctions::IsTaskReserved(pendingReservationList, (short) child->getTotalTaskId());
    if (result == NO_RESERVATION_FOUND) {
        std::vector<int> parent_ids = application_list[child->getApplicationId()]->getInEdgesID(
                child->getId());

        child->setState(task_state::reserved);
        reservedChild = std::make_shared<TaskMapping>(TaskMapping((int) parent_ids.size(), child, mapping_type::in_progress));
        reservedChild->parent_mappings.push_back(parent_mapping);
        reservedChild->setMappingType(mapping_type::reservation);
        pendingReservationList.push_back(reservedChild);
        result = (int) pendingReservationList.size() - 1;
    } else {
        reservedChild = pendingReservationList[result];
        reservedChild->parent_mappings.push_back(parent_mapping);
    }

    if ((AlgorithmFlag::algorithm_mode & FLAG_PROACTIVE_ALGORITHM) && parent_mapping->getFinishValue() > reservedChild->getInputUploadStart())
        reservedChild->setInputUploadStart(parent_mapping->getFinishValue());

    return result;
}

void PreallocationFunctions::preallocateChild(int child_mapping_index, float start_time,
                                              Graph<ComputationNode, std::shared_ptr<EdgeData>> &network_graph,
                                              std::vector<std::shared_ptr<TaskMapping>> &in_progress,
                                              std::vector<std::shared_ptr<TaskMapping>> &reservation_queue,
                                              std::vector<std::shared_ptr<TaskMapping>> &pendingReservationList,
                                              unsigned long dag_node_count) {

    std::shared_ptr<TaskMapping> child_mapping = pendingReservationList[child_mapping_index];
    std::vector<float> resource_release_times = HelperFunctions::generateResourceReleaseTimes(start_time, in_progress,
                                                                                              reservation_queue);

    for (const float &finish_time: resource_release_times)
        if (AlgorithmServices::ChooseNode(network_graph, child_mapping, finish_time + 1, dag_node_count))
            break;

    NetworkTopologyServices::addUploadsToLink(child_mapping, network_graph);
    network_graph.vertices.at(child_mapping->getComputationNodeId())->taskQueue.push_back((child_mapping));
    reservation_queue.push_back(child_mapping);

    /* NEED MANAGE PENDING QUEUE POST PARTITION ALLOCATION
     * OTHERWISE INDEX OUT OF BOUNDS */
    if (AlgorithmFlag::algorithm_mode & FLAG_PREALLOCATION_ALGORITHM)
        pendingReservationList.erase(pendingReservationList.begin() + child_mapping_index);
}
