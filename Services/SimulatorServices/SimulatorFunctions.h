//
// Created by Jamie Cotter on 28/07/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_SIMULATORFUNCTIONS_H
#define FIRSTHOPMOBILEOFFLOADING_SIMULATORFUNCTIONS_H


#include <Models/Task/Task.h>
#include <Models/VertexData/NetworkVertexData.h>

class SimulatorFunctions {
public:
    static bool isValidNode(const Task &task, const NetworkVertexData &vt, std::pair<float, float> pair);

    static void
    programLoop(NetworkTopology &network, std::vector<ApplicationEvent> incoming_applications, float completion_time, char* output_file_name);

    static void checkIncomingApplications(
            std::vector<std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> *total_task_list,
            std::vector<ApplicationEvent> *applications, float current_time);

    static void UpdateEventList(std::vector<TaskMapping> &inProgress, std::vector<TaskMapping> &finished, float &time,
                                float completion_time, std::vector<ApplicationEvent> *pVector);

    static std::vector<TaskMapping> sortEventList(std::vector<TaskMapping> eventList);

    static std::pair<std::pair<int, float>, std::pair<float, float>> ChooseNode(
            std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> &networkList,
            Task &task, float current_time, NetworkTopology &topology, std::vector<float> vector,
            std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> map, std::vector<std::pair<float, float>> &data_transfer_times);

    static std::pair<std::pair<float, float>, std::pair<float, float>>
    calculateRunTime(Task &task, int source_node_index, int currentNodeIndex,
                     std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> &networkList,
                     float current_time, NetworkTopology &network, std::vector<float> upload_vector,
                     std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> edge_map,
                     std::vector<std::pair<float, float>> &tmp_data_transfer_times);

    static std::vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
    getReadyTasks(
            std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> &taskList);

    static void processReadyTasks(
            std::vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> *readyTaskList,
            std::vector<std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> *total_task_lists);

    static void taskMapping(float time, NetworkTopology &network, std::vector<TaskMapping> *inProgress,
                            TaskVertexData &selectedTask,
                            std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> &networkVertexList,
                            std::vector<std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> &total_task_lists,
                            std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> &map);

    static void runAlgorithm(
            std::vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> &readyTaskList,
            std::vector<std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> &total_task_lists,
            std::vector<TaskMapping> &inProgress,
            std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> &networkVertexList,
            NetworkTopology &network, float time,
            std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> &map);

    static float getInputTasksSize(
            std::vector<std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
            &total_task_lists,
            TaskVertexData &selectedTask);

};


#endif //FIRSTHOPMOBILEOFFLOADING_SIMULATORFUNCTIONS_H
