//
// Created by Jamie Cotter on 20/07/2021.
//

#include "main.h"
#include <Services/NetworkTopologyServices/NetworkTopologyServices.h>
#include <Services/ApplicationTopologyServices/ApplicationTopologyServices.h>


using namespace std;

int main() {
    NetworkTopology network = NetworkTopologyServices::generateNetwork();
    NetworkTopologyServices::logInfo(network);

    cout << endl << "NAVIGATOR " << endl;
    ApplicationTopology navigator = ApplicationTopologyServices::generateNavigator();
    ApplicationTopologyServices::logInfo(navigator);

    main::programLoop(network, navigator);

    return 0;
}

void main::programLoop(NetworkTopology &network, ApplicationTopology &navigator) {
    //Retrieving the list of vertices and their edges(in & out) in both our generated application and our network
    auto navigatorVertexList = ApplicationTopologyServices::getVertices(navigator);
    auto networkVertexList = NetworkTopologyServices::getVertices(network);

    //Will hold the tasks ready to be offloaded in each loop
    vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
            readyTaskList;

    //Will hold the nodes ready to be offloaded in each loop
    vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
            readyNodeList;

    //Contains a list of tasks mapped to nodes currently computing
    vector<TaskMapping> inProgress;

    //Contains a finished list of tasks
    vector<TaskMapping> finished;

    //Represents time in seconds
    float time = 0.0f;

    int size;

    while(finished.size() != navigatorVertexList.size()) {
        //Sorting the current event list from most recent to oldest time
        inProgress = main::sortEventList(inProgress);

        //Moving to the next event
        if (inProgress.size() != 0) {
            TaskMapping tM = inProgress.back();
            inProgress.pop_back();

            time = tM.absoluteFinish;
            tM.task.get().task.get().setDone(true);
            tM.task.get().task.get().setInProgress(false);

            if (tM.node.get().type == mobile)
                tM.node.get().mobileNode->setIsFree(true);
            else if (tM.node.get().type == cloud)
                tM.node.get().comp->setIsFree(true);
            else if (tM.node.get().type == node_type::edge)
                tM.node.get().edgeNode->setIsFree(true);

            finished.push_back(tM);
        }

        readyTaskList = main::getReadyTasks(navigatorVertexList);
        readyNodeList = main::getReadyNodes(networkVertexList);

        int count = (readyTaskList.size() < readyNodeList.size()) ? readyTaskList.size() : readyNodeList.size();

        for (int i = 0; i < count; i++) {
            auto &selectedTask = readyTaskList.back().get().m_property;
            readyTaskList.pop_back();

            auto &selectedNode = readyNodeList.back().get().m_property;
            readyNodeList.pop_back();

            selectedTask.task.get().setInProgress(true);

            if (selectedNode.type == mobile)
                selectedNode.mobileNode->setIsFree(false);
            else if (selectedNode.type == cloud) {
                selectedNode.comp->setIsFree(false);
            } else if (selectedNode.type == node_type::edge)
                selectedNode.edgeNode->setIsFree(false);

            float finish = time + main::calculateProcessingTime(selectedTask, selectedNode);
            inProgress.push_back({time, finish, selectedTask, selectedNode});
        }

    }

    main::logResults(finished);

    return;
}

void main::logResults(vector<TaskMapping> finished){
    cout << endl << "LOGGING MAPPING RESULTS:" << endl;
    for(auto taskMapping : finished){
        cout << "====================" << endl;
        cout << "TASK: " << endl;
        cout << taskMapping.task.get().task->to_string() << endl;
        cout << "VERTEX:" << endl;

        if(taskMapping.node.get().type == cloud)
            cout << taskMapping.node.get().comp->to_string();
        else if(taskMapping.node.get().type == node_type::edge)
            cout << taskMapping.node.get().edgeNode->to_string();
        else if(taskMapping.node.get().type == mobile)
            cout << taskMapping.node.get().mobileNode->to_string();

        cout << endl << "START TIME: " << taskMapping.absoluteStart << endl;
        cout << "FINISH TIME: " << taskMapping.absoluteFinish << endl;
    }
}

vector<TaskMapping> main::sortEventList(vector<TaskMapping> eventList) {
    sort(std::begin(eventList), std::end(eventList),
         [](TaskMapping taskMapA, TaskMapping taskMapB) {
             return taskMapA.absoluteFinish > taskMapB.absoluteFinish;
         });

    return eventList;
}

/**
 * Function returns the time it takes for a node to process a task (currently basic)
 *
 * @param task - Task that we retrieve the MI count from
 * @param node - Node that we retrieves the MIPS count from
 * @return - Time derived from the millions of instructions / millions of instructions per second.
 */
float main::calculateProcessingTime(TaskVertexData task, NetworkVertexData node) {
    float mi = 0.0f;
    float mips = 0.0f;
    switch (node.type) {
        case cloud:
            mi += task.task->getMillionsOfInstructions();
            mips += node.comp->getMillionsInstructionsPerCore();
            break;
        case mobile:
            mi += task.task->getMillionsOfInstructions();
            mips += node.mobileNode->getMillionsInstructionsPerCore();
            break;
        case node_type::edge:
            mi += task.task->getMillionsOfInstructions();
            mips += node.edgeNode->getMillionsInstructionsPerCore();
            break;
        default:
            break;
    }
    float res = (mi / mips);
    return res;
}

/**
 * @param taskList - The list of tasks that represent all the nodes in a network
 * @return - A vector containing only the nodes not busy
 */
vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
main::getReadyNodes(
        vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> &networkList) {

    vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> res;
    for (auto &vertex : networkList) {
        if (vertex.m_property.type == mobile) {
            if (vertex.m_property.mobileNode->isFree())
                res.push_back(vertex);
        } else if (vertex.m_property.type == cloud) {
            if (vertex.m_property.comp->isFree())
                res.push_back(vertex);
        } else if (vertex.m_property.type == node_type::edge) {
            if (vertex.m_property.edgeNode->isFree())
                res.push_back(vertex);
        }
    }

    return res;
}

/**
 * @param taskList - The list of tasks that represent all the vertices in an application
 * @return - A vector containing only the tasks ready to be offloaded
 */
vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
main::getReadyTasks(
        vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> &taskList) {

    vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>> res;
    for (auto &vertex : taskList) {
        Task& tempTask = vertex.m_property.task.get();
        if (!vertex.m_property.task->isDone() && !vertex.m_property.task->isInProgress()) {
            if (vertex.m_in_edges.size() == 0) {
                res.push_back(vertex);
                continue;
            } else {
                bool ready = true;
                for (auto edge : vertex.m_in_edges) {
                    Task& edgeTemp = taskList[edge.m_target].m_property.task.get();
                    if (!taskList[edge.m_target].m_property.task->isDone() ||
                        taskList[edge.m_target].m_property.task->isInProgress()) {
                        ready = false;
                    }
                }
                if (ready)
                    res.push_back(vertex);
            }
        }
    }

    return res;
}

