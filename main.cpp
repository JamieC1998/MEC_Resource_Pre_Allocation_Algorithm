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

void main::programLoop(NetworkTopology& network, ApplicationTopology& navigator){
    auto navigatorVertexList = ApplicationTopologyServices::getVertices(navigator);
    vector<detail::adj_list_gen<adjacency_list<vecS, vecS, directedS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, directedS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> taskList;

    taskList.insert(std::end(taskList), std::begin(navigatorVertexList), std::end(navigatorVertexList));

    auto readyList = main::getReadyTasks(taskList);
}

vector<detail::adj_list_gen<adjacency_list<vecS, vecS, directedS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, directedS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>
        main::getReadyTasks(vector<detail::adj_list_gen<adjacency_list<vecS, vecS, directedS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, directedS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> &taskList){

//    cout << "WHAMMO";
//    for(auto vertex : taskList){
//        if(!vertex.m_property.done){
//            if(vertex.m_out_edges.s)
//        }
//    }
    return taskList;
}

