//
// Created by Jamie Cotter on 26/01/2022.
//

#ifndef UNTITLED2_ALGORITHMSERVICES_H
#define UNTITLED2_ALGORITHMSERVICES_H

#include <memory>
#include "../../Model/ComputationNode/ComputationNode.h"
#include "../../Graph/EdgeData/EdgeData.h"
#include "../../Graph/Graph.h"

class AlgorithmServices {
public:
    static void runAlgorithm(std::vector<std::shared_ptr<Task>> &readyTaskList,
                             std::vector<std::shared_ptr<Graph<Task, bool>>> &applications,
                             std::vector<std::shared_ptr<TaskMapping>> &inProgress,
                             Graph<ComputationNode, std::shared_ptr<EdgeData>> &network_graph,
                             std::vector<std::shared_ptr<TaskMapping>> &reservationQueue, float time,
                             std::vector<std::shared_ptr<TaskMapping>> &finished,
                             std::vector<std::shared_ptr<TaskMapping>> &pendingReservationQueue);

    static void taskMapping(float time, Graph<ComputationNode, std::shared_ptr<EdgeData>> &network_graph,
                            std::vector<std::shared_ptr<TaskMapping>> &inProgress,
                            const std::shared_ptr<Task> &selectedTask,
                            std::vector<std::shared_ptr<TaskMapping>> &reservationQueue,
                            std::vector<std::shared_ptr<Graph<Task, bool>>> &applications,
                            std::vector<std::shared_ptr<TaskMapping>> &finished,
                            std::vector<std::shared_ptr<TaskMapping>> &pendingReservationQueue);

    static bool ChooseNode(Graph<ComputationNode, std::shared_ptr<EdgeData>> &network_graph,
                           const std::shared_ptr<TaskMapping> &task,
                           float current_time, unsigned long i);

    static std::pair<float, float>
    calculateRunTime(std::shared_ptr<TaskMapping> task, std::shared_ptr<ComputationNode> node,
                     Graph<ComputationNode, std::shared_ptr<EdgeData>> &network_graph, float time,
                     std::unordered_map<int, std::vector<std::shared_ptr<std::pair<float, float>>>> &upload_windows);

    static std::unordered_map<int, std::vector<std::shared_ptr<std::pair<float, float>>>>
    calculateParentUploadTimes(float current_time, std::shared_ptr<TaskMapping> tm,
                               const std::shared_ptr<ComputationNode> &node,
                               Graph<ComputationNode, std::shared_ptr<EdgeData>> &network_graph, float &ot_up);

    static bool isValidNode(std::shared_ptr<TaskMapping> task, std::pair<float, float> time_window,
                            const std::shared_ptr<ComputationNode>& node, unsigned long i);

    static std::unordered_map<int, std::vector<std::shared_ptr<std::pair<float, float>>>>
    calculateOutputReturnTime(float time, const std::shared_ptr<TaskMapping> &task,
                              const std::shared_ptr<ComputationNode> &computationNode,
                              Graph<ComputationNode, std::shared_ptr<EdgeData>> &network_graph,
                              std::unordered_map<int, std::vector<std::shared_ptr<std::pair<float, float>>>> upload_windows,
                              float process_finish);
};


#endif //UNTITLED2_ALGORITHMSERVICES_H
