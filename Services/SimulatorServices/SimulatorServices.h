//
// Created by Jamie Cotter on 23/01/2022.
//

#ifndef UNTITLED2_SIMULATORSERVICES_H
#define UNTITLED2_SIMULATORSERVICES_H


#include "../../Model/ComputationNode/ComputationNode.h"
#include "../../Graph/EdgeData/EdgeData.h"
#include "../../Graph/Graph.h"
#include "../../Model/Task/Task.h"

namespace SimulatorServices {
    std::vector<std::shared_ptr<TaskMapping>>
    programLoop(Graph<ComputationNode, std::shared_ptr<EdgeData>> &network,
                std::vector<std::pair<float, std::shared_ptr<Graph<Task, bool>>>> &incoming_applications,
                float simulation_time,
                std::vector<std::shared_ptr<Graph<Task, bool>>> &applications);

    void sortTaskMappingList(std::vector<std::shared_ptr<TaskMapping>> &taskMapping);

    void UpdateEvents(std::vector<std::shared_ptr<TaskMapping>> &inProgress,
                      std::vector<std::shared_ptr<TaskMapping>> &finished,
                      float &time, std::vector<std::shared_ptr<TaskMapping>> &reservationList,
                      float completion_time,
                      std::vector<std::pair<float, std::shared_ptr<Graph<Task, bool>>>> &incoming_applications,
                      std::vector<std::shared_ptr<Graph<Task, bool>>> &applications,
                      Graph<struct ComputationNode, std::shared_ptr<EdgeData>> &networkGraph);

    void checkIncomingApplications(std::vector<std::shared_ptr<Graph<Task, bool>>> &applications,
                                   std::vector<std::pair<float, std::shared_ptr<Graph<Task, bool>>>> &incoming_applications,
                                   float &current_time, float d);
}


#endif //UNTITLED2_SIMULATORSERVICES_H
