//
// Created by Jamie Cotter on 28/07/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_SIMULATORFUNCTIONS_H
#define FIRSTHOPMOBILEOFFLOADING_SIMULATORFUNCTIONS_H


#include <Models/Task/Task.h>
#include <Models/VertexData/NetworkVertexData.h>

class SimulatorFunctions {
public:
    static bool isValidNode(const Task &task, const NetworkVertexData &vt);

    static void programLoop(NetworkTopology &network, std::vector<ApplicationEvent> incoming_applications, float completion_time);

    static void checkIncomingApplications(
            std::vector<std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> *total_task_list,
            std::vector<ApplicationEvent> *applications, float current_time);

    static void UpdateEventList(std::vector<TaskMapping> &inProgress, std::vector<TaskMapping> &finished, float &time);

    static std::vector<TaskMapping> sortEventList(std::vector<TaskMapping> eventList);

    static std::pair<int, float> ChooseNode(
            std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> &networkList,
            Task &task, float current_time, NetworkTopology &topology);

    static float calculateRunTime(Task &task, int source_node_index, int currentNodeIndex,
                                  std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> &networkList,
                                  float current_time, NetworkTopology &network);

    static std::vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
    getReadyTasks(
            std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> &taskList);
};


#endif //FIRSTHOPMOBILEOFFLOADING_SIMULATORFUNCTIONS_H
