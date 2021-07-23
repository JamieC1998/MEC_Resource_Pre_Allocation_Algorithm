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

    vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
            readyTaskList;

    vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
        readyNodeList;

    //Contains a list of tasks mapped to nodes currently computing
    vector<TaskMapping> inProgress;

    //Contains a finished list of tasks
    vector<TaskMapping> finished;

    //Represents time in seconds
    unsigned long long int time;

    do {
        readyTaskList = main::getReadyTasks(navigatorVertexList);
        readyNodeList = main::getReadyNodes(networkVertexList);

    } while (readyTaskList.size() != 0);

    return;
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
            if(vertex.m_property.mobileNode->isFree())
                res.push_back(vertex);
        }
        else if(vertex.m_property.type == cloud){
            if(vertex.m_property.comp->isFree())
                res.push_back(vertex);
        }
        else if(vertex.m_property.type == node_type::edge){
            if(vertex.m_property.edgeNode->isFree())
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
        if (!vertex.m_property.task->isDone()) {
            if (vertex.m_in_edges.size() == 0) {
                res.push_back(vertex);
                continue;
            } else {
                bool ready = true;
                for (auto edge : vertex.m_in_edges) {
                    if (!taskList[edge.m_target].m_property.task->isDone()) {
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

