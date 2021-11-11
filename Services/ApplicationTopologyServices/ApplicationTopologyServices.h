//
// Created by Jamie Cotter on 22/07/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_APPLICATIONTOPOLOGYSERVICES_H
#define FIRSTHOPMOBILEOFFLOADING_APPLICATIONTOPOLOGYSERVICES_H

#include <boost/graph/adjacency_list.hpp>
#include <Models/VertexData/TaskVertexData.h>
#include <Models/VertexData/EdgePropertyData.h>

using namespace boost;

typedef property<boost::edge_weight_t, int> EdgeWeightProperty;
typedef adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, EdgeWeightProperty> ApplicationTopology;
typedef graph_traits<ApplicationTopology>::edge_iterator application_edge_iterator;
typedef graph_traits<ApplicationTopology>::vertex_iterator application_vertex_iterator;
typedef graph_traits<ApplicationTopology>::vertex_descriptor application_vertex_descriptor;

struct ApplicationEvent {
    float ready_time = 0;
    ApplicationTopology application;
};

class ApplicationTopologyServices {

public:

    static void logInfo(ApplicationTopology &application);

    static std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>
    getVertices(ApplicationTopology &application);

    static ApplicationTopology generateNavigator(int source_mobile_id);

    static ApplicationTopology generateChessApp(int source_mobile_id);

    static ApplicationTopology generateApplications(
            std::pair<float, std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>> item,
            int source_mobile_id);

    static float calculateLowerBound(std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> &taskList, int super_node_MI);
};


#endif //FIRSTHOPMOBILEOFFLOADING_APPLICATIONTOPOLOGYSERVICES_H
