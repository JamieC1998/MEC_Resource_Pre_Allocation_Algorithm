//
// Created by Jamie Cotter on 23/01/2022.
//

#include <vector>
#include <iostream>
#include "SimulatorServices.h"
#include "../AlgorithmServices/AlgorithmServices.h"
#include "../../utils/HelperFunctions/HelperFunctions.h"

std::vector<std::shared_ptr<TaskMapping>>
SimulatorServices::programLoop(Graph<ComputationNode, std::shared_ptr<EdgeData>> &network,
                               std::vector<std::pair<float, std::shared_ptr<Graph<Task, bool>>>> &incoming_applications,
                               float simulation_time,
                               std::vector<std::shared_ptr<Graph<Task, bool>>> &applications) {
    int total_task_count = 0;

    for (const auto &application: incoming_applications)
        total_task_count += (int) application.second->vertices.size();

    //Time in milliseconds
    float time = 0.0f;

    std::vector<std::shared_ptr<Task>> readyTaskList;

    std::vector<std::shared_ptr<TaskMapping>> inProgress;

    std::vector<std::shared_ptr<TaskMapping>> reservationList;

    std::vector<std::shared_ptr<TaskMapping>> pendingReservationList;

    std::vector<std::shared_ptr<TaskMapping>> finished;

    while (!(time >= simulation_time || (finished.size() == total_task_count && incoming_applications.empty()))) {

        SimulatorServices::sortTaskMappingList(reservationList);

        SimulatorServices::UpdateEvents(inProgress, finished, time, reservationList, simulation_time,
                                        incoming_applications, applications, network);

        AlgorithmServices::runAlgorithm(readyTaskList, applications, inProgress, network, reservationList, time,
                                        finished, pendingReservationList);
    }

    return finished;
}

void SimulatorServices::sortTaskMappingList(std::vector<std::shared_ptr<TaskMapping>> &taskMapping) {
    sort(std::begin(taskMapping), std::end(taskMapping),
         [](const std::shared_ptr<TaskMapping> &taskMapA, const std::shared_ptr<TaskMapping> &taskMapB) {
             float task_a_val = taskMapA->getSortTime();
             float task_b_val = taskMapB->getSortTime();

             return (taskMapA->getMappingType() == taskMapB->getMappingType()) ? task_a_val < task_b_val
                                                                               : (taskMapA->getMappingType() ==
                                                                                  reservation) ? task_a_val <
                                                                                                 task_b_val :
                                                                                 task_a_val <= task_b_val;
         });
}

void SimulatorServices::UpdateEvents(std::vector<std::shared_ptr<TaskMapping>> &inProgress,
                                     std::vector<std::shared_ptr<TaskMapping>> &finished,
                                     float &time, std::vector<std::shared_ptr<TaskMapping>> &reservationList,
                                     float completion_time,
                                     std::vector<std::pair<float, std::shared_ptr<Graph<Task, bool>>>> &incoming_applications,
                                     std::vector<std::shared_ptr<Graph<Task, bool>>> &applications,
                                     Graph<struct ComputationNode, std::shared_ptr<EdgeData>> &networkGraph) {

    std::vector<std::shared_ptr<TaskMapping>> combinedList;
    combinedList.reserve(inProgress.size() + reservationList.size());
    combinedList.insert(combinedList.end(), inProgress.begin(), inProgress.end());
    combinedList.insert(combinedList.end(), reservationList.begin(), reservationList.end());

    SimulatorServices::sortTaskMappingList(combinedList);

    if ((!incoming_applications.empty() && combinedList.empty()) ||
        (!incoming_applications.empty() && incoming_applications.front().first <= combinedList.front()->getSortTime()))
        checkIncomingApplications(applications, incoming_applications, time, completion_time);
    else if (combinedList.front()->getSortTime() <= completion_time) {
        std::shared_ptr<TaskMapping> tm = combinedList.front();

        time = tm->getSortTime();
        if (tm->getMappingType() == in_progress) {
            tm->getTask()->setState(processed);
            tm->setMappingType(mapping_type::finished);
            finished.push_back(tm);
            HelperFunctions::removeMappingFromList(networkGraph.vertices.at(tm->getComputationNodeId())->taskQueue, tm);
            HelperFunctions::removeMappingFromList(inProgress, tm);
        }
    } else if (combinedList.front()->getSortTime() > completion_time)
        time = completion_time;
}

void SimulatorServices::checkIncomingApplications(std::vector<std::shared_ptr<Graph<Task, bool>>> &applications,
                                                  std::vector<std::pair<float, std::shared_ptr<Graph<Task, bool>>>> &incoming_applications,
                                                  float &current_time, float completion_time) {

    if (incoming_applications.front().first > completion_time)
        current_time = completion_time;
    else {
        applications.push_back(incoming_applications.front().second);
        incoming_applications.erase(incoming_applications.begin());
    }
}


