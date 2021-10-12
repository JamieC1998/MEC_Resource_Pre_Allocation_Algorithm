//
// Created by Jamie Cotter on 20/07/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_MAIN_H
#define FIRSTHOPMOBILEOFFLOADING_MAIN_H

#include <Services/NetworkTopologyServices/NetworkTopologyServices.h>
#include <Services/ApplicationTopologyServices/ApplicationTopologyServices.h>

struct TaskMapping {
    float absoluteStart = 0;
    float absoluteFinish = 0;
    std::reference_wrapper<TaskVertexData> task;
    std::reference_wrapper<NetworkVertexData> node;
    float processStartTime = 0;
    float processFinishTime = 0;
};

class main {
public:
    static void programLoop(NetworkTopology &network, ApplicationTopology &navigator);

    static std::vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
    getReadyNodes(
            std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> &networkList);

    static float calculateProcessingTime(TaskVertexData task, NetworkVertexData node);

    static void logResults(const std::vector<TaskMapping> &finished, char* output_file_name);
};


#endif //FIRSTHOPMOBILEOFFLOADING_MAIN_H
