//
// Created by Jamie Cotter on 20/07/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_MAIN_H
#define FIRSTHOPMOBILEOFFLOADING_MAIN_H

#include <Services/NetworkTopologyServices/NetworkTopologyServices.h>
#include <Services/ApplicationTopologyServices/ApplicationTopologyServices.h>

class main {
public:

    static void programLoop(NetworkTopology &network, ApplicationTopology &navigator);

    static std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, directedS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, directedS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>
    getReadyTasks(
            std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, directedS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, directedS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> &taskList);
};


#endif //FIRSTHOPMOBILEOFFLOADING_MAIN_H
