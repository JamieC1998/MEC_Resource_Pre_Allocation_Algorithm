//
// Created by Jamie Cotter on 22/07/2021.
//
#include <boost/graph/adjacency_list.hpp>
#include <Models/VertexData/NetworkVertexData.h>

#ifndef FIRSTHOPMOBILEOFFLOADING_NETWORKTOPOLOGYSERVICES_H
#define FIRSTHOPMOBILEOFFLOADING_NETWORKTOPOLOGYSERVICES_H

using namespace boost;

typedef property<edge_weight_t, int> EdgeWeightProperty;
typedef adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, EdgeWeightProperty> NetworkTopology;
typedef graph_traits<NetworkTopology>::edge_iterator network_edge_iterator;
typedef graph_traits<NetworkTopology>::vertex_iterator network_vertex_iterator;
typedef graph_traits<NetworkTopology>::vertex_descriptor network_vertex_descriptor;
typedef property_map<NetworkTopology, vertex_index_t>::type IdMap;

class NetworkTopologyServices {
public:
    static NetworkTopology generateNetwork();
    static void logInfo(NetworkTopology& network);
    static std::vector<int> shortestRouteBetweenTwoPoints(int start, int goal, std::vector<network_vertex_descriptor> pred);
    static std::pair<std::vector<unsigned long>, iterator_property_map<std::__wrap_iter<int *>, vec_adj_list_vertex_id_map<NetworkVertexData, unsigned long>, int, int &>>
    shortestPaths(int start, NetworkTopology& network);
};
#endif //FIRSTHOPMOBILEOFFLOADING_NETWORKTOPOLOGYSERVICES_H
