//
// Created by Jamie Cotter on 28/07/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_SIMULATORFUNCTIONS_H
#define FIRSTHOPMOBILEOFFLOADING_SIMULATORFUNCTIONS_H

class SimulatorFunctions {
public:
    static bool isValidNode(Task task, const NetworkVertexData &vt, std::pair<float, float> timeWindow,
                            std::vector<ApplicationGraph> &vector);

    static void
    programLoop(NetworkTopology &network, std::vector<ApplicationEvent> incoming_applications, float completion_time,
                char *output_file, char *lower_bound_file_name);

    static void checkIncomingApplications(
            std::vector<ApplicationGraph> &total_task_list,
            std::vector<ApplicationEvent> *applications, float current_time, std::vector<float> &lower_bound_times,
            const SuperNode& super_node);

    static void UpdateEventList(std::vector<TaskMapping> &inProgress, std::vector<TaskMapping> &finished, float &time,
                                std::vector<ReservationMapping> reservationQueue, float completion_time,
                                std::vector<ApplicationEvent> *applications, std::vector<ApplicationGraph>& total_task_lists);

    static std::vector<TaskMapping> sortEventList(std::vector<TaskMapping> eventList);

    static std::pair<int, float> ChooseNode(
            std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> &networkList,
            Task task, float current_time, NetworkTopology &topology, const std::vector<TaskMapping> &parents,
            float &startTime, std::vector<std::pair<float, float>> &finish_times_list,
            std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> &map,
            std::vector<ApplicationGraph> &vector);

    static float calculateRunTime(Task task, int currentNodeIndex,
                                  std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> &networkList,
                                  float current_time, NetworkTopology &network, std::vector<TaskMapping> parents,
                                  float &startTime, std::vector<std::pair<float, float>> &tmp_finish_times,
                                  std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> map,
                                  std::vector<ApplicationGraph> &vector);

    static std::vector<std::pair<int, int>> getReadyTasks(ApplicationGraph taskGraph);

    static std::vector<std::pair<int, bool>> processReadyTasks(std::vector<std::pair<int, int>> &readyTaskList, std::vector<ApplicationGraph> &total_task_lists);

    static void taskMapping(float time, NetworkTopology &network, std::vector<TaskMapping> *inProgress,
                            std::pair<int, int> selectedTask,
                            std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> &networkVertexList,
                            std::vector<ReservationMapping> &reservationQueue,
                            std::vector<ApplicationGraph> &total_task_lists,
                            std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> &map);

    static void runAlgorithm(
            std::vector<std::pair<int, int>> &readyTaskList,
            std::vector<ApplicationGraph> &total_task_lists,
            std::vector<TaskMapping> &inProgress,
            std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> &networkVertexList,
            NetworkTopology &network, float time, std::vector<ReservationMapping> &reservationQueue,
            std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> &map);

    static int checkIfTaskReserved(std::vector<ReservationMapping> &reservationQueue, std::pair<int, int> selectedTask);

    static std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex *>
    getOutGoingTasks(
            std::vector<std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
            &total_task_lists,
            TaskVertexData &selectedTask);

    static void preallocateTasks(
            std::vector<ApplicationGraph> &total_task_lists,
            std::pair<int, int> selectedTask,
            std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> &networkVertexList,
            std::vector<ReservationMapping> &reservationQueue, NetworkTopology &network, float time, TaskMapping map,
            std::vector<TaskMapping> *inProgress,
            std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> &unorderedMap);

    static std::vector<ReservationMapping> sortReservations(std::vector<ReservationMapping> reservationMappings);
};


#endif //FIRSTHOPMOBILEOFFLOADING_SIMULATORFUNCTIONS_H
