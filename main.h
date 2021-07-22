//
// Created by Jamie Cotter on 20/07/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_MAIN_H
#define FIRSTHOPMOBILEOFFLOADING_MAIN_H

#include <boost/graph/adjacency_list.hpp>
#include <Models/NetworkVertexData.h>

using namespace boost;

typedef property<edge_weight_t, int> EdgeWeightProperty;
typedef adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, EdgeWeightProperty> NetworkTopology;
typedef graph_traits<NetworkTopology>::edge_iterator network_edge_iterator;
typedef graph_traits<NetworkTopology>::vertex_iterator network_vertex_iterator;
typedef graph_traits<NetworkTopology>::vertex_descriptor network_vertex_descriptor;
typedef property_map<NetworkTopology, vertex_index_t>::type IdMap;

class main {
public:
    static NetworkTopology generateNetwork();
    static void logInfo(NetworkTopology& network);
    static std::vector<int> shortestRouteBetweenTwoPoints(int start, int goal, NetworkTopology& network);
};


#endif //FIRSTHOPMOBILEOFFLOADING_MAIN_H
