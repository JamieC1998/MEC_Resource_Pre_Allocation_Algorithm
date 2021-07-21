//
// Created by Jamie Cotter on 20/07/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_MAIN_H
#define FIRSTHOPMOBILEOFFLOADING_MAIN_H

#include <boost/graph/adjacency_list.hpp>
#include <Models/NetworkVertexData.h>

typedef boost::property<boost::edge_weight_t, int> EdgeWeightProperty;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, NetworkVertexData, EdgeWeightProperty> NetworkTopology;
typedef boost::graph_traits<NetworkTopology>::edge_iterator edge_iterator;
typedef boost::graph_traits<NetworkTopology>::vertex_iterator vertex_iterator;

class main {
public:
    static NetworkTopology generateNetwork();
    static void logInfo(NetworkTopology& network);
};


#endif //FIRSTHOPMOBILEOFFLOADING_MAIN_H
