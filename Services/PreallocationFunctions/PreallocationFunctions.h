//
// Created by Jamie Cotter on 05/02/2022.
//

#ifndef UNTITLED2_PREALLOCATIONFUNCTIONS_H
#define UNTITLED2_PREALLOCATIONFUNCTIONS_H


#include "../../Graph/Graph.h"
#include "../../Model/ComputationNode/ComputationNode.h"
#include "../../Graph/EdgeData/EdgeData.h"

namespace PreallocationFunctions {
    void preallocateChildren(Graph<ComputationNode, std::shared_ptr<EdgeData>> &network,
                             std::shared_ptr<TaskMapping> &parent_mapping,
                             std::vector<std::shared_ptr<TaskMapping>> &reservation_queue,
                             std::vector<std::shared_ptr<Graph<Task, bool>>> &application_list,
                             std::vector<std::shared_ptr<TaskMapping>> &inProgressList,
                             std::vector<std::shared_ptr<TaskMapping>> &pendingReservationList);

    int
    createUpdateChildReservation(
            std::shared_ptr<TaskMapping> &parent_mapping,
            std::vector<std::shared_ptr<Graph<Task, bool>>> &application_list,
            std::shared_ptr<Task> &child,
            std::vector<std::shared_ptr<TaskMapping>> &pendingReservationList);

    void preallocateChild(int child_mapping_index, float start_time,
                                 Graph<ComputationNode, std::shared_ptr<EdgeData>> &network_graph,
                                 std::vector<std::shared_ptr<TaskMapping>> &in_progress,
                                 std::vector<std::shared_ptr<TaskMapping>> &reservation_queue,
                                 std::vector<std::shared_ptr<TaskMapping>> &pendingReservationList, unsigned long i);
};


#endif //UNTITLED2_PREALLOCATIONFUNCTIONS_H
