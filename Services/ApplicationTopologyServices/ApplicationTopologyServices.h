//
// Created by Jamie Cotter on 22/07/2021.
//


#include <Models/VertexData/TaskVertexData.h>
#include <boost/graph/adjacency_list.hpp>

#ifndef FIRSTHOPMOBILEOFFLOADING_APPLICATIONTOPOLOGYSERVICES_H
#define FIRSTHOPMOBILEOFFLOADING_APPLICATIONTOPOLOGYSERVICES_H

using namespace boost;

typedef property<boost::edge_weight_t, int> EdgeWeightProperty;
typedef adjacency_list<vecS, vecS, directedS, TaskVertexData, EdgeWeightProperty> ApplicationTopology;
typedef graph_traits<ApplicationTopology>::edge_iterator application_edge_iterator;
typedef graph_traits<ApplicationTopology>::vertex_iterator application_vertex_iterator;
typedef graph_traits<ApplicationTopology>::vertex_descriptor application_vertex_descriptor;

class ApplicationTopologyServices {

public:
    static ApplicationTopology generateNavigator();

    static void logInfo(ApplicationTopology &application);

    static std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, directedS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, directedS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>
    getVertices(ApplicationTopology &application);
};


#endif //FIRSTHOPMOBILEOFFLOADING_APPLICATIONTOPOLOGYSERVICES_H
