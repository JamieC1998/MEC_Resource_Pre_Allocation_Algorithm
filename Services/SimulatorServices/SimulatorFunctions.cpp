//
// Created by Jamie Cotter on 28/07/2021.
//

#include <Services/NetworkTopologyServices/NetworkTopologyServices.h>
#include <Services/ApplicationTopologyServices/ApplicationTopologyServices.h>
#include <main.h>
#include <thread>
#include "SimulatorFunctions.h"
#include <Models/Mappings/ReservationMapping.h>

using namespace std;
using namespace boost;

/**
 * This function checks if a given node is a viable deployment candidate for a task
 * @param task - The task to be deployed
 * @param vt - The node that is being assessed
 * @return - A boolean value indicating true if it is a valid deployment candidate, false if it isn't
 */
bool SimulatorFunctions::isValidNode(const Task &task, const NetworkVertexData &vt, pair<float, float> timeWindow) {
    ComputationNode node = (vt.type == mobile) ? vt.mobileNode.get() : (vt.type == cloud) ? vt.comp.get()
                                                                                          : vt.edgeNode.get();

    float total_ram_usage = 0.0f;
    float total_storage_usage = 0.0f;
    int total_cores_used = 0;

    for (const auto &item: node.getReservations()) {
        Task tempTask = item.task;
        if (timeWindow.first <= item.timeWindow.second && item.timeWindow.first <= timeWindow.second) {
            total_ram_usage += item.task.getRam();
            total_storage_usage += item.task.getStorage();
            total_cores_used += item.task.getCoreCount();
        }
    }

    total_ram_usage = 0.0f;
    total_storage_usage = 0.0f;
    total_cores_used = 0;

    for (const auto &item: node.getTaskVector()) {
        if (timeWindow.first <= item.timeWindow.second && item.timeWindow.first <= timeWindow.second) {
            total_ram_usage += item.task.getRam();
            total_storage_usage += item.task.getStorage();
            total_cores_used += item.task.getCoreCount();
        }
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
        Task &task, float current_time, NetworkTopology &topology, vector<TaskMapping> parents, float &startTime) {

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
        int current_node_index = ((int) std::distance(networkList.begin(), vertex));
        float tempStartTime = 0.0f;
        float current_run_time = SimulatorFunctions::calculateRunTime(task, source_node_index, current_node_index,
                                                                      networkList, current_time, topology, parents,
                                                                      tempStartTime);

        std::pair<float, float> applicationTimeRange = make_pair(tempStartTime, current_run_time);

        if (SimulatorFunctions::isValidNode(task, vertex->m_property, applicationTimeRange)) {
            if (current_run_time < min_run_time) {
                index = current_node_index;
                min_run_time = current_run_time;
                startTime = tempStartTime;
            }
        }

    }

    return make_pair(index, min_run_time);
}

float SimulatorFunctions::calculateRunTime(Task &task, int source_node_index, int currentNodeIndex,
                                           vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> &networkList,
                                           float current_time, NetworkTopology &network,
                                           vector<TaskMapping> parents, float &startTime) {

    ComputationNode current_node = (networkList[currentNodeIndex].m_property.type == mobile)
                                   ? networkList[currentNodeIndex].m_property.mobileNode.get()
                                   : (networkList[currentNodeIndex].m_property.type == cloud)
                                     ? networkList[currentNodeIndex].m_property.comp.get()
                                     : networkList[currentNodeIndex].m_property.edgeNode.get();


    float rt_local = (float) task.getMillionsOfInstructions() / (float) current_node.getMillionsInstructionsPerCore();

    float ot_up = 0.0f;

    //If the task is not allowed to be offloaded and this is the source task we do not need to calculate bandwidth
    //as there's no data to transfer
    if (source_node_index == currentNodeIndex)
        ot_up += current_time;
    else {
        float bandwidth = NetworkTopologyServices::getBandwidth(source_node_index, currentNodeIndex, network);
        auto mi = (float) (task.getMillionsOfInstructions() * 1000000);
        ot_up = ((mi * INSTRUCTION_SIZE_MEGABYTES) / bandwidth) + current_time;
    }

    for (auto &parent : parents) {
        float bw = NetworkTopologyServices::getBandwidth(parent.nodeIndex, currentNodeIndex, network);
        float tmp_ot_up = (bw == 0) ? (parent.absoluteFinish + 0.0001f) :
                          (parent.task.get().task->getDataOut() / bw) + (parent.absoluteFinish);
        if (tmp_ot_up > ot_up) ot_up = tmp_ot_up;
    }

    startTime = ot_up;

    return rt_local + ot_up;
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

                  return !a.get().m_property.task.get().isOffload();
              });

}

int
SimulatorFunctions::checkIfTaskReserved(vector<ReservationMapping> &reservationQueue, TaskVertexData &selectedTask) {
    int index = -1;

    for (int i = 0; i < reservationQueue.size(); i++) {
        if (selectedTask.task.get().getId() == reservationQueue[i].task->task->getId()) {
            index = i;
            break;
        }
    }
    return index;
}

void SimulatorFunctions::taskMapping(
        float time,
        NetworkTopology &network,
        vector<TaskMapping> *inProgress,
        TaskVertexData &selectedTask,
        vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> &networkVertexList,
        vector<ReservationMapping> &reservationQueue,
        vector<vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
        &total_task_lists
) {

    int result = SimulatorFunctions::checkIfTaskReserved(reservationQueue, selectedTask);
    int nodeIndex = -1;

    float estimatedFinishTime = -1.0f;
    float startTime = 0;

    NetworkVertexData *selectedNode;

    //If the task is pre-allocated
    if (result != -1) {
        auto &reservedTask = reservationQueue[result];

        // If our reservation isn't ready
        if(reservedTask.startTime != time)
            return;

        nodeIndex = reservationQueue[result].nodeIndex;
        selectedNode = &networkVertexList[(int) nodeIndex].m_property;
        estimatedFinishTime = reservedTask.finishTime;
        startTime = reservedTask.startTime;

        //Upcasting our selected node to base class for convenience
        ComputationNode &node = (networkVertexList[nodeIndex].m_property.type == mobile)
                                ? networkVertexList[nodeIndex].m_property.mobileNode.get()
                                : (networkVertexList[nodeIndex].m_property.type ==
                                   cloud)
                                  ? networkVertexList[nodeIndex].m_property.comp.get()
                                  : networkVertexList[nodeIndex].m_property.edgeNode.get();


        int index = -1;
        auto &reservations = const_cast<vector<NodeMapping> &>(node.getReservations());
        for (int i = 0; i < reservations.size(); i++) {
            if (reservations[i].task.getId() == reservedTask.task->task->getId()) {
                index = i;
                break;
            }
        }

        reservations.erase(std::begin(reservations) + index);
        reservationQueue.erase(std::begin(reservationQueue) + result);
    } else {
        vector<TaskMapping> parents;
        pair<int, float> selectedNodeData = SimulatorFunctions::ChooseNode(networkVertexList,
                                                                           selectedTask.task.get(), time,
                                                                           network, parents, startTime);

        //If a valid node has not been found
        if (selectedNodeData.first == -1)
            return;

        estimatedFinishTime = selectedNodeData.second;
        selectedNode = &networkVertexList[selectedNodeData.first].m_property;
        nodeIndex = selectedNodeData.first;
    }

    selectedTask.task.get().setInProgress(true);

    //Upcasting our selected node to base class for convenience
    ComputationNode &node = (selectedNode->type == mobile) ? selectedNode->mobileNode.get()
                                                           : (selectedNode->type ==
                                                              cloud)
                                                             ? selectedNode->comp.get()
                                                             : selectedNode->edgeNode.get();



    //Adding the selected task to our selected node
    (const_cast<vector<struct NodeMapping> &>(node.getTaskVector())).push_back(
            {selectedTask.task.get(), make_pair(startTime, estimatedFinishTime)});
    TaskMapping map{startTime, estimatedFinishTime, selectedTask, selectedNode, nodeIndex};
    inProgress->push_back(map);

    SimulatorFunctions::preallocateTasks(total_task_lists, selectedTask, networkVertexList, reservationQueue, network,
                                         time, map, inProgress);
}

void SimulatorFunctions::preallocateTasks(
        vector<vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
        &total_task_lists,
        TaskVertexData &selectedTask,
        vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> &networkVertexList,
        vector<ReservationMapping> &reservationQueue,
        NetworkTopology &network,
        float time,
        TaskMapping map,
        vector<TaskMapping> *inProgress) {

    auto outGoingTasks = SimulatorFunctions::getOutGoingTasks(total_task_lists, selectedTask);

    for (auto &outGoingTask : outGoingTasks) {

        int index = -1;
        for (int i = 0; i < reservationQueue.size(); i++) {
            if (reservationQueue[i].task->task->getId() == outGoingTask->m_property.task->getId()) {
                index = i;
            }
        }

        if (index != -1)
            reservationQueue[index].parents.push_back(map);
        else {
            std::vector<TaskMapping> parents;
            parents.push_back(map);


            ReservationMapping reservedNode{&outGoingTask->m_property, -1, -1, -1, parents,
                                            static_cast<int>(outGoingTask->m_in_edges.size())};

            //1st index is the node it is allocated to, second is the time it will take, third is the task, 4th is placeholder
            reservationQueue.push_back(reservedNode);
            index = reservationQueue.size() - 1;
        }
        if (reservationQueue[index].parents.size() == reservationQueue[index].parentCount) {
            float startTime = 0.0f;
            auto chooseNode = SimulatorFunctions::ChooseNode(networkVertexList, outGoingTask->m_property.task.get(),
                                                             map.absoluteStart,
                                                             network, reservationQueue[index].parents,
                                                             startTime);

            //If we cannot preallocate a node for when our last task in our parents ends
            //Then we must attempt preallocate, fast forwarding through the state of the network till
            //a node can be preallocated.
            if (chooseNode.first == -1) {
                for (int i = 0; i < inProgress->size(); i++) {
                    if (reservationQueue[index].parents[reservationQueue[index].parents.size() - 1].absoluteStart >=
                        ((vector<TaskMapping>) *inProgress)[i].absoluteFinish)
                        continue;

                    startTime = 0.0f;
                    chooseNode = SimulatorFunctions::ChooseNode(networkVertexList, outGoingTask->m_property.task.get(),
                                                                ((vector<TaskMapping>) *inProgress)[i].absoluteFinish +
                                                                0.0001f,
                                                                network, reservationQueue[index].parents,
                                                                startTime);

                    if (chooseNode.first != -1)
                        break;
                }
                if (chooseNode.first == -1) {
                    for (int i = 0; i < reservationQueue.size(); i++) {
                        if (reservationQueue[index].task->task->getId() == reservationQueue[i].task->task->getId())
                            continue;

                        if (reservationQueue[index].parents[reservationQueue[index].parents.size() -
                                                            1].absoluteStart >=
                            reservationQueue[i].finishTime)
                            continue;

                        startTime = 0.0f;
                        chooseNode = SimulatorFunctions::ChooseNode(networkVertexList, outGoingTask->m_property.task.get(),
                                                                    ((vector<TaskMapping>) *inProgress)[i].absoluteFinish +
                                                                    0.0001f,
                                                                    network, reservationQueue[index].parents,
                                                                    startTime);

                        if(chooseNode.first != -1)
                            break;

                    }
                }
            }

            ComputationNode &node = (networkVertexList[chooseNode.first].m_property.type == mobile)
                                    ? networkVertexList[chooseNode.first].m_property.mobileNode.get()
                                    : (networkVertexList[chooseNode.first].m_property.type ==
                                       cloud)
                                      ? networkVertexList[chooseNode.first].m_property.comp.get()
                                      : networkVertexList[chooseNode.first].m_property.edgeNode.get();

            ReservationMapping &reservationItem = reservationQueue[index];
            reservationItem.startTime = startTime;
            reservationItem.finishTime = chooseNode.second;
            reservationItem.nodeIndex = chooseNode.first;

            vector<NodeMapping> nodeReservations = node.getReservations();
            nodeReservations.push_back({reservationItem.task->task.get(), make_pair(startTime, chooseNode.second)});
            node.setReservations(nodeReservations);
        }
    }
}

vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex *>
SimulatorFunctions::getOutGoingTasks(
        vector<vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
        &total_task_lists,
        TaskVertexData &selectedTask) {
    vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex *> res;

    for (auto &total_task_list : total_task_lists) {
        for (auto &task_item : total_task_list) {
            if (task_item.m_property.task.get().getId() == selectedTask.task->getId()) {
                for (auto &m_out_edge : task_item.m_out_edges)
                    res.push_back(&total_task_list[m_out_edge.m_target]);
                break;
            }
        }
    }

    return res;
}

void SimulatorFunctions::runAlgorithm(
        vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
        &readyTaskList,
        vector<vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
        &total_task_lists,
        vector<TaskMapping> &inProgress,
        vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>
        &networkVertexList,
        NetworkTopology &network,
        float time,
        vector<ReservationMapping> &reservationQueue) {

    SimulatorFunctions::processReadyTasks(&readyTaskList, &total_task_lists);

    int task_count = ((int) readyTaskList.size());

    for (int i = 0; i < task_count; i++) {
        TaskVertexData &selectedTask = readyTaskList.back().get().m_property;
        readyTaskList.pop_back();

        SimulatorFunctions::taskMapping(time, network, &inProgress, selectedTask, networkVertexList, reservationQueue,
                                        total_task_lists);
    }
}

vector<ReservationMapping> SimulatorFunctions::sortReservations(vector<ReservationMapping> reservationMappings) {
    sort(std::begin(reservationMappings), std::end(reservationMappings),
         [](ReservationMapping taskMapA, ReservationMapping taskMapB) {
             return taskMapA.startTime > taskMapB.startTime;
         });
    return reservationMappings;
}

void SimulatorFunctions::programLoop(NetworkTopology &network, vector<ApplicationEvent> incoming_applications,
                                     float completion_time) {
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

    vector<ReservationMapping> reservationQueue;

    while (!(time >= completion_time || (finished.size() == total_task_count && incoming_applications.empty()))) {
        //Sorting the current event list from most recent to oldest time
        inProgress = SimulatorFunctions::sortEventList(inProgress);
        reservationQueue = SimulatorFunctions::sortReservations(reservationQueue);

        //Moving to the next event
        SimulatorFunctions::UpdateEventList(inProgress, finished, time, reservationQueue);

        SimulatorFunctions::runAlgorithm(readyTaskList, total_task_lists, inProgress, networkVertexList, network, time,
                                         reservationQueue);

        /**
         * If there are no tasks in progress and no tasks ready, that means that
         * we are waiting for the next application to enter
         */
        if (inProgress.empty() && readyTaskList.empty() && !incoming_applications.empty()) {
            auto min_time = (float) INT_MAX;
            for (auto &incoming_application : incoming_applications) {
                if (incoming_application.ready_time < min_time)
                    min_time = incoming_application.ready_time;
            }
            time = min_time;
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
void SimulatorFunctions::UpdateEventList(vector<TaskMapping> &inProgress, vector<TaskMapping> &finished, float &time,
                                         vector<ReservationMapping> reservationMapping) {
    vector<ReservationMapping> tmp;
    std::copy_if(reservationMapping.begin(),
                 reservationMapping.end(),
                 std::back_inserter(tmp),
                 [](ReservationMapping mapping) {
                     return mapping.startTime != -1;
                 });

    if (((!inProgress.empty() && !tmp.empty()) &&
         tmp.back().startTime <= inProgress.back().absoluteFinish)
        || (!tmp.empty() && inProgress.empty())) {
        time = tmp.back().startTime;
    }
    else if (!inProgress.empty()) {
        TaskMapping tM = inProgress.back();
        inProgress.pop_back();

        time = tM.absoluteFinish;
        tM.task.get().task.get().setDone(true);
        tM.task.get().task.get().setInProgress(false);

        ComputationNode &node = (tM.node->type == mobile) ? tM.node->mobileNode.get() : (tM.node->type ==
                                                                                         cloud)
                                                                                        ? tM.node->comp.get()
                                                                                        : tM.node->edgeNode.get();

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
