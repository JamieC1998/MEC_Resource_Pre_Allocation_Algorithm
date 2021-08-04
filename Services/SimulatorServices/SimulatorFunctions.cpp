//
// Created by Jamie Cotter on 28/07/2021.
//

#include <Services/NetworkTopologyServices/NetworkTopologyServices.h>
#include <Services/ApplicationTopologyServices/ApplicationTopologyServices.h>
#include <main.h>
#include "SimulatorFunctions.h"

using namespace std;
using namespace boost;

/**
 * This function checks if a given node is a viable deployment candidate for a task
 * @param task - The task to be deployed
 * @param vt - The node that is being assessed
 * @return - A boolean value indicating true if it is a valid deployment candidate, false if it isn't
 */
bool SimulatorFunctions::isValidNode(const Task &task, const NetworkVertexData &vt) {
    ComputationNode node = (vt.type == mobile) ? vt.mobileNode.get() : (vt.type == cloud) ? vt.comp.get()
                                                                                          : vt.edgeNode.get();

    float total_ram_usage = 0.0f;
    float total_storage_usage = 0.0f;
    int total_cores_used = 0;

    for (const auto &item: node.getTaskVector()) {
        total_ram_usage += item.getRam();
        total_storage_usage += item.getStorage();
        total_cores_used += item.getCoreCount();
    }

    //If the node must be processed locally, we check if this is the source node.
    if (!task.isOffload() && vt.type == mobile) {
        if (node.getId() != task.getSourceMobileId())
            return false;

        else {
            if ((total_ram_usage + task.getRam()) > node.getRam())
                return false;
            if ((total_storage_usage + task.getStorage()) > node.getStorage())
                return false;
            if ((total_cores_used + task.getCoreCount()) > node.getCores())
                return false;
        }
    }

        //If the task can't be offloaded but the node in question is not the mobile
    else if (!task.isOffload() && vt.type != mobile) {
        return false;
    }

        //Checking if it meets the resource requirements
    else {
        if ((total_ram_usage + task.getRam()) > node.getRam())
            return false;
        if ((total_storage_usage + task.getStorage()) > node.getStorage())
            return false;
        if ((total_cores_used + task.getCoreCount()) > node.getCores())
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
pair<int, float> SimulatorFunctions::ChooseNode(
        vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> &networkList,
        Task &task, float current_time, NetworkTopology &topology) {

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

    for (auto vertex = networkList.begin(); vertex != networkList.end(); vertex++) {
        if (SimulatorFunctions::isValidNode(task, vertex->m_property)) {
            int current_node_index = ((int) std::distance(networkList.begin(), vertex));

            float current_run_time = SimulatorFunctions::calculateRunTime(task, source_node_index, current_node_index,
                                                                          networkList, current_time, topology);

            if (current_run_time < min_run_time) {
                index = current_node_index;
                min_run_time = current_run_time;
            }
        }

    }

    return make_pair(index, min_run_time);
}

float SimulatorFunctions::calculateRunTime(Task &task, int source_node_index, int currentNodeIndex,
                                           vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> &networkList,
                                           float current_time, NetworkTopology &network) {

    ComputationNode current_node = (networkList[currentNodeIndex].m_property.type == mobile)
                                   ? networkList[currentNodeIndex].m_property.mobileNode.get()
                                   : (networkList[currentNodeIndex].m_property.type == cloud)
                                     ? networkList[currentNodeIndex].m_property.comp.get()
                                     : networkList[currentNodeIndex].m_property.edgeNode.get();


    float rt_local = (float) task.getMillionsOfInstructions() / (float) current_node.getMillionsInstructionsPerCore();

    //If the task is not allowed to be offloaded and this is the source task we do not need to calculate bandwidth
    //as there's no data to transfer
    if (source_node_index == currentNodeIndex)
        return current_time += rt_local;

    float bandwidth = NetworkTopologyServices::getBandwidth(source_node_index, currentNodeIndex, network);
    auto mi = (float) (task.getMillionsOfInstructions() * 1000000);
    float ot_up = ((mi * INSTRUCTION_SIZE_MEGABYTES) + ((float) task.getDataIn())) / bandwidth;
    float ot_down = ((float) task.getDataOut()) / bandwidth;

    return current_time + rt_local + ot_up + ot_down;
}

void SimulatorFunctions::programLoop(NetworkTopology &network, vector<ApplicationEvent *> incoming_applications,
                                     float completion_time) {
    int total_task_count = 0;

    for (auto &incoming_application : incoming_applications)
        total_task_count += ((int) incoming_application->application.m_vertices.size());

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

    while (!(time >= completion_time || (finished.size() == total_task_count && incoming_applications.empty()))) {
        //Sorting the current event list from most recent to oldest time
        inProgress = SimulatorFunctions::sortEventList(inProgress);

        //Moving to the next event
        SimulatorFunctions::UpdateEventList(inProgress, finished, time);

        //Iterating through each application queue
        for (auto &taskList: total_task_lists) {
            readyTaskList = SimulatorFunctions::getReadyTasks(taskList);

            int task_count = ((int) readyTaskList.size());

            for (int i = 0; i < task_count; i++) {
                auto &selectedTask = readyTaskList.back().get().m_property;
                readyTaskList.pop_back();

                pair<int, float> selectedNodeData = SimulatorFunctions::ChooseNode(networkVertexList,
                                                                                   selectedTask.task.get(), time,
                                                                                   network);

                if (selectedNodeData.first == -1)
                    continue;

                auto &selectedNode = networkVertexList[selectedNodeData.first].m_property;

                selectedTask.task.get().setInProgress(true);

                //Upcasting our selected node to base class for convenience
                ComputationNode &node = (selectedNode.type == mobile) ? selectedNode.mobileNode.get()
                                                                      : (selectedNode.type ==
                                                                         cloud)
                                                                        ? selectedNode.comp.get()
                                                                        : selectedNode.edgeNode.get();

                //Adding the selected task to our selected node
                (const_cast<vector<struct Task> &>(node.getTaskVector())).push_back(selectedTask.task.get());

                //Adding our mapping to the event list
                inProgress.push_back({time, selectedNodeData.second, selectedTask, selectedNode});
            }
        }

        SimulatorFunctions::checkIncomingApplications(&total_task_lists, &incoming_applications, time);
    }

    main::logResults(finished);
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
        vector<ApplicationEvent *> *applications,
        float current_time) {

    vector<ApplicationEvent *> &incoming_application = *applications;
    for (int i = 0; i < incoming_application.size(); i++) {
        if (((float) incoming_application[i]->ready_time) <= current_time) {
            total_task_list->push_back(ApplicationTopologyServices::getVertices(incoming_application[i]->application));
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
void SimulatorFunctions::UpdateEventList(vector<TaskMapping> &inProgress, vector<TaskMapping> &finished, float &time) {
    if (!inProgress.empty()) {
        TaskMapping tM = inProgress.back();
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
            if (tM.task.get().task->getId() == node.getTaskVector()[i].getId()) {
                task_index_to_rem = i;
                break;
            }
        }

        (const_cast<vector<struct Task> &> (node.getTaskVector())).erase(
                node.getTaskVector().begin() + task_index_to_rem);

        finished.push_back(tM);
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
