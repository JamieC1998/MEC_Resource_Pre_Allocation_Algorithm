//
// Created by Jamie Cotter on 22/07/2021.
//


#ifndef FIRSTHOPMOBILEOFFLOADING_NETWORKTOPOLOGYSERVICES_H
#define FIRSTHOPMOBILEOFFLOADING_NETWORKTOPOLOGYSERVICES_H

#include <boost/graph/adjacency_list.hpp>
#include <Models/VertexData/NetworkVertexData.h>
#include <Models/VertexData/EdgePropertyData.h>
#include <Models/Mappings/SuperNode.h>

using namespace boost;

typedef property<edge_weight_t, EdgePropertyData> EdgeWProperty;
typedef adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, EdgeWProperty> NetworkTopology;
typedef graph_traits<NetworkTopology>::edge_iterator network_edge_iterator;
typedef graph_traits<NetworkTopology>::vertex_iterator network_vertex_iterator;
typedef graph_traits<NetworkTopology>::vertex_descriptor network_vertex_descriptor;
typedef property_map<NetworkTopology, vertex_index_t>::type IdMap;

const int SERVERS_PER_CLOUDLET = 10;

class NetworkTopologyServices {
public:
    static NetworkTopology generateNetwork();

    static void logInfo(NetworkTopology &network);

    static std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex>
    getVertices(NetworkTopology &network);

    static float getBandwidth(int source, int destination, NetworkTopology &network);

    static SuperNode getSuperNode(std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> networkVertexList);

    static std::vector<std::pair<float, float>> getActiveUploads(int source, int destination, float start_time, NetworkTopology& g);

    static void addUploadsToLink(const std::pair<std::vector<std::pair<float, float>>, std::vector<TaskMapping>> &parentList, int destination_node,
                     NetworkTopology &network, std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> &map);

    static std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> generateEdgeMap(NetworkTopology& network, int node_count);

    static std::pair<float, float>
    findLinkSlot(std::vector<std::pair<float, float>> occupancy_times, float start_time, float data_size,
                 float bandwidth,
                 float d);

};

#endif //FIRSTHOPMOBILEOFFLOADING_NETWORKTOPOLOGYSERVICES_H
