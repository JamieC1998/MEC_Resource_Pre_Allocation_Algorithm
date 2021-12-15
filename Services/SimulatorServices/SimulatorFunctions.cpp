//
// Created by Jamie Cotter on 28/07/2021.
//

#include <Services/NetworkTopologyServices/NetworkTopologyServices.h>
#include <Services/ApplicationTopologyServices/ApplicationTopologyServices.h>
#include <main.h>
#include <thread>
#include "SimulatorFunctions.h"

using namespace std;
using namespace boost;

/**
 * This function checks if a given node is a viable deployment candidate for a task
 * @param task - The task to be deployed
 * @param vt - The node that is being assessed
 * @return - A boolean value indicating true if it is a valid deployment candidate, false if it isn't
 */
bool
SimulatorFunctions::isValidNode(const Task &task, const NetworkVertexData &vt, std::pair<float, float> timeWindow) {
    ComputationNode node = (vt.type == mobile) ? vt.mobileNode.get() : (vt.type == cloud) ? vt.comp.get()
                                                                                          : vt.edgeNode.get();

    struct ResourceEvent {
        int cpu_usage;
        float ram_usage;
        float storage;

        bool is_increase;
        float time;
    };

    vector<ResourceEvent> resource_events;

    for (const auto &item: node.getTaskVector()) {
        Task tempTask = item.task;
        if (timeWindow.first <= item.timeWindow.second && item.timeWindow.first <= timeWindow.second) {
            for (int i = 0; i < 2; i++) {
                resource_events.push_back({
                                                  1,
                                                  tempTask.getRam(),
                                                  tempTask.getStorage(),
                                                  (bool) i,
                                                  ((bool) i) ? (item.timeWindow.first < timeWindow.first)
                                                               ? timeWindow.first : item.timeWindow.first
                                                             : (item.timeWindow.second > timeWindow.second)
                                                               ? timeWindow.second : item.timeWindow.second
                                          });
            }
        }
    }

    sort(std::begin(resource_events), std::end(resource_events),
         [](const ResourceEvent &a, const ResourceEvent &b) -> bool {
             return a.time < b.time;
         });

    float max_ram_usage = 0;
    float max_storage_usage = 0;
    int max_core_usage = 0;

    float current_ram_usage = 0;
    float current_storage_usage = 0;
    int current_core_usage = 0;

    for (const auto &resource_event: resource_events) {
        if (resource_event.is_increase) {
            current_core_usage += resource_event.cpu_usage;
            current_storage_usage += resource_event.storage;
            current_ram_usage += resource_event.ram_usage;

            if (current_core_usage > max_core_usage)
                max_core_usage = current_core_usage;
            if (current_ram_usage > max_ram_usage)
                max_ram_usage = current_ram_usage;
            if (current_storage_usage > max_storage_usage)
                max_storage_usage = current_storage_usage;
        } else {
            current_core_usage -= resource_event.cpu_usage;
            current_storage_usage -= resource_event.storage;
            current_ram_usage -= resource_event.ram_usage;
        }
    }

    //If the node must be processed locally, we check if this is the source node.
    if (!task.isOffload() && vt.type == mobile) {
        if (node.getId() != task.getSourceMobileId())
            return false;

        else {
            if ((max_ram_usage + task.getRam()) > node.getRam())
                return false;
            if ((max_storage_usage + task.getStorage()) / 1000 > node.getStorage())
                return false;
            if ((max_core_usage + 1) > node.getCores())
                return false;
        }
    }

        //If the task can't be offloaded but the node in question is not the mobile
    else if (!task.isOffload() && vt.type != mobile) {
        return false;
    }

        //Checking if it meets the resource requirements
    else {
        if ((max_ram_usage + task.getRam()) > node.getRam())
            return false;
        if ((max_storage_usage + task.getStorage()) / 1000 > node.getStorage())
            return false;
        if ((max_core_usage + 1) > node.getCores())
            return false;
    }
    return true;
}

/**
 * This function iterates through list of nodes
 * retrieves the run time for a task on each node
 * selects the node with the minimum run time.
 *
 * @param networkList - List of all the vertices in our topology
 * @param task - Task that is to be offloaded
 * @param current_time - The current time in the simulation
 * @param topology -  The graph representing the network
 * @return - Returns the index of the node with the lowest run time and the estimated time of completion
 */
std::pair<std::pair<int, float>, std::pair<float, float>> SimulatorFunctions::ChooseNode(
        std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> &networkList,
        Task &task, float current_time, NetworkTopology &topology, std::vector<float> upload_inputs,
        std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> edge_map,
        std::vector<pair<float, float>> &data_transfer_times) {

    int index = -1;
    float min_run_time = ((float) INT_MAX);

    int source_node_index = 0;

    //Get the graph index of the source device of the task
    for (auto vertex = networkList.begin(); vertex != networkList.end(); vertex++) {
        ComputationNode temp = (vertex->m_property.type == mobile) ? vertex->m_property.mobileNode.get()
                                                                   : (vertex->m_property.type == cloud)
                                                                     ? vertex->m_property.comp.get()
                                                                     : vertex->m_property.edgeNode.get();

        if (temp.getId() == task.getSourceMobileId()) {
            source_node_index = ((int) std::distance(networkList.begin(), vertex));
            break;
        }
    }
    pair<float, float> minProcessTime = make_pair(0.0f, 0.0f);
    for (auto vertex = networkList.begin(); vertex != networkList.end(); vertex++) {
        int current_node_index = ((int) std::distance(networkList.begin(), vertex));
        vector<pair<float, float>> tmp_data_transfer_times;
        pair<pair<float, float>, pair<float, float>> applicationTimeRange = SimulatorFunctions::calculateRunTime(task,
                                                                                                                 source_node_index,
                                                                                                                 current_node_index,
                                                                                                                 networkList,
                                                                                                                 current_time,
                                                                                                                 topology,
                                                                                                                 upload_inputs,
                                                                                                                 edge_map,
                                                                                                                 tmp_data_transfer_times);

        if (SimulatorFunctions::isValidNode(task, vertex->m_property, applicationTimeRange.second)) {

            if (applicationTimeRange.first.second < min_run_time) {
                index = current_node_index;
                min_run_time = applicationTimeRange.first.second;
                minProcessTime = applicationTimeRange.second;
                data_transfer_times = tmp_data_transfer_times;
            }
        }

    }

    return make_pair(make_pair(index, min_run_time), minProcessTime);
}

std::pair<std::pair<float, float>, std::pair<float, float>>
SimulatorFunctions::calculateRunTime(Task &task, int source_node_index, int currentNodeIndex,
                                     std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> &networkList,
                                     float current_time, NetworkTopology &network, std::vector<float> upload_times,
                                     std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> edge_map,
                                     std::vector<pair<float, float>> &tmp_data_transfer_times) {

    ComputationNode current_node = (networkList[currentNodeIndex].m_property.type == mobile)
                                   ? networkList[currentNodeIndex].m_property.mobileNode.get()
                                   : (networkList[currentNodeIndex].m_property.type == cloud)
                                     ? networkList[currentNodeIndex].m_property.comp.get()
                                     : networkList[currentNodeIndex].m_property.edgeNode.get();


    float rt_local = (float) task.getProcessTime(current_node.getType()) + 0.001;

    //If the task is not allowed to be offloaded and this is the source task we do not need to calculate bandwidth
    //as there's no data to transfer
    if (source_node_index == currentNodeIndex)
        return make_pair(make_pair(current_time, current_time + rt_local),
                         make_pair(current_time, current_time + rt_local));

    float bandwidth = NetworkTopologyServices::getBandwidth(source_node_index, currentNodeIndex, network);
    float ot_up = current_time;

    EdgePropertyData &edge = edge_map.at(source_node_index).at(currentNodeIndex);
    float ot_down = 0;
    if(current_node.getType() != node_type::mobile) {
        for (float upload_time : upload_times) {
            pair<float, float> window = NetworkTopologyServices::findLinkSlot(edge.occupancy_times, current_time,
                                                                              upload_time, bandwidth, edge.latency);
            edge.occupancy_times.push_back(window);
            tmp_data_transfer_times.push_back(window);

            if (ot_up < window.second)
                ot_up = window.second;
        }

        pair<float, float> ot_down_window = NetworkTopologyServices::findLinkSlot(edge.occupancy_times, rt_local + ot_up,
                                                                                  task.getDataOut(), bandwidth, 0);


        tmp_data_transfer_times.push_back(ot_down_window);
        ot_down = ot_down_window.second;
    }

    return make_pair(make_pair(current_time, ot_up + rt_local + ot_down),
                     make_pair(ot_up, ot_up + rt_local));
}

void SimulatorFunctions::processReadyTasks(
        vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> *readyTaskList,
        vector<vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> *total_task_lists) {
    //Emptying the task list
    readyTaskList->clear();

    //Iterating through each application queue
    for (auto &taskList: *total_task_lists) {
        auto tempRes = SimulatorFunctions::getReadyTasks(taskList);
        readyTaskList->insert(std::end(*readyTaskList), std::begin(tempRes), std::end(tempRes));
    }

    //Sorting readyTasks to prioritise tasks that cannot be offloaded
    std::sort(readyTaskList->begin(), readyTaskList->end(),
              [](std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> a,
                 std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> b) {

                  return a.get().m_property.task.get().isOffload() > b.get().m_property.task.get().isOffload();
              });

}

void SimulatorFunctions::taskMapping(float time, NetworkTopology &network, std::vector<TaskMapping> *inProgress,
                                     TaskVertexData &selectedTask,
                                     std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> &networkVertexList,
                                     std::vector<std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> &total_task_lists,
                                     std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> &map) {

    vector<float> upload_inputs = ApplicationTopologyServices::getParentData(total_task_lists,
                                                                             selectedTask.task.get().getId());

    vector<pair<float, float>> data_transfer_times;
    Task task = selectedTask.task.get();

    pair<pair<int, float>, pair<float, float>> selectedNodeData = SimulatorFunctions::ChooseNode(networkVertexList,
                                                                                                 selectedTask.task.get(),
                                                                                                 time,
                                                                                                 network,
                                                                                                 upload_inputs, map,
                                                                                                 data_transfer_times);

    //If a valid node has not been found
    if (selectedNodeData.first.first == -1)
        return;


    auto &selectedNode = networkVertexList[selectedNodeData.first.first].m_property;

    selectedTask.task.get().setInProgress(true);

    //Upcasting our selected node to base class for convenience
    ComputationNode &node = (selectedNode.type == mobile) ? selectedNode.mobileNode.get()
                                                          : (selectedNode.type ==
                                                             cloud)
                                                            ? selectedNode.comp.get()
                                                            : selectedNode.edgeNode.get();

    //Adding the selected task to our selected node
    (const_cast<vector<struct NodeMapping> &>(node.getTaskVector())).push_back(
            {selectedTask.task.get(), make_pair(selectedNodeData.second.first, selectedNodeData.second.second)});

    inProgress->push_back(
            {time, selectedNodeData.first.second, selectedTask, selectedNode, selectedNodeData.second.first,
             selectedNodeData.second.second});

    NetworkTopologyServices::addUploadsToLink(data_transfer_times, 0, selectedNodeData.first.first, network, map);
}

void SimulatorFunctions::runAlgorithm(
        std::vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> &readyTaskList,
        std::vector<std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> &total_task_lists,
        std::vector<TaskMapping> &inProgress,
        std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> &networkVertexList,
        NetworkTopology &network, float time, std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> &map) {

    SimulatorFunctions::processReadyTasks(&readyTaskList, &total_task_lists);

    int task_count = ((int) readyTaskList.size());

    for (int i = 0; i < task_count; i++) {
        TaskVertexData &selectedTask = readyTaskList.back().get().m_property;
        float inputSize = SimulatorFunctions::getInputTasksSize(total_task_lists, selectedTask);
        selectedTask.task.get().setDataIn(inputSize);
        readyTaskList.pop_back();

        SimulatorFunctions::taskMapping(time, network, &inProgress, selectedTask, networkVertexList, total_task_lists,
                                        map);
    }
}

void SimulatorFunctions::programLoop(NetworkTopology &network, vector<ApplicationEvent> incoming_applications,
                                     float completion_time, char *output_file_name) {
    int total_task_count = 0;

    for (auto &incoming_application : incoming_applications)
        total_task_count += ((int) incoming_application.application.m_vertices.size());

    //Represents time in seconds
    float time = 0.0f;

    //Will contain queue of tasks for each application currently offloading
    vector<vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> total_task_lists;

    //Retrieving the list of vertices and their edges(in & out) in both our generated application and our network
    auto networkVertexList = NetworkTopologyServices::getVertices(network);

    //Will hold the tasks ready to be offloaded in each loop
    vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
            readyTaskList;

    //Contains a list of tasks mapped to nodes currently computing
    vector<TaskMapping> inProgress;

    //Contains a finished list of tasks
    vector<TaskMapping> finished;

    std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> edge_data = NetworkTopologyServices::generateEdgeMap(
            network, networkVertexList.size());

    while (!(time >= completion_time || (finished.size() == total_task_count && incoming_applications.empty()))) {
        //Sorting the current event list from most recent to oldest time
        inProgress = SimulatorFunctions::sortEventList(inProgress);

        //Moving to the next event
        SimulatorFunctions::UpdateEventList(inProgress, finished, time, completion_time, &incoming_applications);

        SimulatorFunctions::checkIncomingApplications(&total_task_lists, &incoming_applications, time);

        SimulatorFunctions::runAlgorithm(readyTaskList, total_task_lists, inProgress, networkVertexList, network, time,
                                         edge_data);
    }

    main::logResults(finished, output_file_name);
}

/**
 * This function is responsible for checking the incoming application queue in order to decide
 * when to begin offloading an application
 *
 * @param total_task_list - A matrix with each top level index containing an applications list of tasks
 * @param applications - The applications yet to be offloaded
 * @param current_time - The current time of the simulation
 */
void SimulatorFunctions::checkIncomingApplications(
        vector<vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> *total_task_list,
        vector<ApplicationEvent> *applications,
        float current_time) {

    vector<ApplicationEvent> &incoming_application = *applications;
    for (int i = 0; i < incoming_application.size(); i++) {
        if (incoming_application[i].ready_time <= current_time) {
            total_task_list->push_back(ApplicationTopologyServices::getVertices(incoming_application[i].application));
            incoming_application.erase(incoming_application.begin() + i);
            i = i - 1;
        }
    }
}

/**
 * This function processes the next event to complete in our event list
 * freeing up the nodes associated so that they can re-enter the ready node pool
 * Adding the result of the event to finished queue
 *
 * @param inProgress - A reference to a vector of task mappings in progress
 * @param finished - A reference to a vector that contains task mappings that are completed
 * @param time - A reference to the current time of the application to be updated with the current event finish time
 */
void SimulatorFunctions::UpdateEventList(std::vector<TaskMapping> &inProgress, std::vector<TaskMapping> &finished,
                                         float &time,
                                         float completion_time, std::vector<ApplicationEvent> *applications) {
    vector<ApplicationEvent> &incoming_applications = *applications;
    /**
     * If there are no tasks in progress and no tasks ready, that means that
     * we are waiting for the next application to enter
     */
    auto app_min_time = (float) INT_MAX;
    bool app_chosen = false;
    if (!incoming_applications.empty()) {
        for (auto &incoming_application : incoming_applications) {
            if (incoming_application.ready_time < app_min_time) {
                app_min_time = incoming_application.ready_time;
                app_chosen = true;
            }
        }
    }

    if (!inProgress.empty()) {
        TaskMapping tM = inProgress.back();
        if (app_chosen && app_min_time < tM.absoluteFinish) {
            if (app_min_time > completion_time) time = completion_time;
            else time = app_min_time;
            return;
        }
        if (tM.absoluteFinish > completion_time) {
            time = completion_time;
            return;
        }

        inProgress.pop_back();

        time = tM.absoluteFinish;
        tM.task.get().task.get().setDone(true);
        tM.task.get().task.get().setInProgress(false);

        ComputationNode &node = (tM.node.get().type == mobile) ? tM.node.get().mobileNode.get() : (tM.node.get().type ==
                                                                                                   cloud)
                                                                                                  ? tM.node.get().comp.get()
                                                                                                  : tM.node.get().edgeNode.get();

        int task_index_to_rem = -1;
        for (int i = 0; i < node.getTaskVector().size(); i++) {
            if (tM.task.get().task->getId() == node.getTaskVector()[i].task.getId()) {
                task_index_to_rem = i;
                break;
            }
        }

        (const_cast<vector<struct NodeMapping> &> (node.getTaskVector())).erase(
                node.getTaskVector().begin() + task_index_to_rem);

        finished.push_back(tM);
    } else if (app_chosen) {
        time = app_min_time;
    }
}

vector<TaskMapping> SimulatorFunctions::sortEventList(vector<TaskMapping> eventList) {
    sort(std::begin(eventList), std::end(eventList),
         [](TaskMapping taskMapA, TaskMapping taskMapB) {
             return taskMapA.absoluteFinish > taskMapB.absoluteFinish;
         });

    return eventList;
}

/**
 * @param taskList - The list of tasks that represent all the vertices in an application
 * @return - A vector containing only the tasks ready to be offloaded
 */
vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
SimulatorFunctions::getReadyTasks(
        vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> &taskList) {

    vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> res;
    for (auto &vertex : taskList) {
        Task &tempTask = vertex.m_property.task.get();
        if (!vertex.m_property.task->isDone() && !vertex.m_property.task->isInProgress()) {
            //If a task depends on no other tasks and is not in progress or completed
            // then it is ready to be offloaded
            if (vertex.m_in_edges.empty()) {
                res.emplace_back(vertex);
                continue;
            } else {
                bool ready = true;
                //Checking to see if the incoming edges(tasks a task depends upon) are completed
                for (auto edge : vertex.m_in_edges) {
                    Task &edgeTemp = taskList[edge.m_target].m_property.task.get();
                    if (!taskList[edge.m_target].m_property.task->isDone() ||
                        taskList[edge.m_target].m_property.task->isInProgress()) {
                        ready = false;
                    }
                }
                if (ready)
                    res.emplace_back(vertex);
            }
        }
    }

    return res;
}

float SimulatorFunctions::getInputTasksSize(
        vector<vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
        &total_task_lists,
        TaskVertexData &selectedTask) {
    vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex *> res;

    float inputTasksSize = 0.0f;
    for (auto &total_task_list : total_task_lists) {
        for (auto &task_item : total_task_list) {
            if (task_item.m_property.task.get().getId() == selectedTask.task->getId()) {
                for (auto &m_in_edge : task_item.m_in_edges)
                    inputTasksSize += total_task_list[m_in_edge.m_target].m_property.task.get().getDataOut();
                break;
            }
        }
    }

    return inputTasksSize;
}