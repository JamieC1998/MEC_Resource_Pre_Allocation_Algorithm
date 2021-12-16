//
// Created by Jamie Cotter on 22/07/2021.
//

#include "NetworkTopologyServices.h"
#include <boost/graph/adjacency_list.hpp>
#include <Models/ComputationNode/ComputationNode.h>
#include <Models/EdgeNode/EdgeNode.h>
#include <Models/MobileNode/MobileNode.h>
#include <Models/VertexData/NetworkVertexData.h>
#include <Models/Mappings/TaskMapping.h>
#include <Models/Mappings/SuperNode.h>

using namespace std;

void NetworkTopologyServices::logInfo(NetworkTopology &network) {
    pair<network_edge_iterator, network_edge_iterator> ei = edges(network);

    pair<network_vertex_iterator, network_vertex_iterator> vi = vertices(network);

    cout << "Number of edges = " << num_edges(network) << "\n";
    cout << "Edge list:\n";

    property_map<NetworkTopology, edge_weight_t>::type weightMap = get(edge_weight, network);
    for (auto iter = ei.first; iter != ei.second; iter++) {
        cout << "edge " << *iter << " weight " << INT_MAX - get(weightMap, *iter).edge_weight << endl;
    }


    cout << endl << "Number of vertices = " << num_vertices(network) << "\n";
    cout << "Vertex list:\n" << endl;

    for (auto iter = vi.first; iter != vi.second; iter++) {
        node_type type = get(&NetworkVertexData::type, network)[(int) *iter];
        if (type == mobile)
            cout << "Vertex " << *iter << ":\n" << get(&NetworkVertexData::mobileNode, network)[*iter]->to_string()
                 << endl;
        else if (type == node_type::edge)
            cout << "Vertex " << *iter << ":\n" << get(&NetworkVertexData::edgeNode, network)[*iter]->to_string()
                 << endl;
        else if (type == cloud)
            cout << "Vertex " << *iter << ":\n" << get(&NetworkVertexData::comp, network)[*iter]->to_string()
                 << endl;
    }
}

float NetworkTopologyServices::getBandwidth(int source, int destination, NetworkTopology &network) {
    if (source == destination)
        return 0;

    property_map<NetworkTopology, edge_weight_t>::type weightMap = get(edge_weight, network);

    auto res = boost::edge(source, destination, network).first;
    return INT_MAX - get(weightMap, res).edge_weight;
}

vector<pair<float, float>>
NetworkTopologyServices::getActiveUploads(int source, int destination, float start_time, NetworkTopology &g) {
    property_map<NetworkTopology, edge_weight_t>::type weight_map = get(edge_weight, g);

    auto res = boost::edge(source, destination, g).first;
    vector<pair<float, float>> upload_events = get(weight_map, res).occupancy_times;
    vector<pair<float, float>> active_tasks;
    for (pair<float, float> upload_event : upload_events) {
        if (upload_event.second >= start_time)
            active_tasks.push_back(upload_event);
    }
    return active_tasks;
}

void
NetworkTopologyServices::addUploadsToLink(
        const std::pair<std::vector<pair<float, float>>, std::vector<TaskMapping>> &parentList,
        int destination_node,
        NetworkTopology &network,
        std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> &map) {

    for (int i = 0; i < parentList.first.size(); i++) {
        int parentNode = parentList.second[i].nodeIndex;

        if (parentNode == destination_node)
            continue;

        EdgePropertyData &edge = (parentNode < destination_node) ? map.at(parentNode).at(destination_node) : map.at(
                destination_node).at(parentNode);
        edge.occupancy_times.emplace_back(parentList.first[i]);
    }
}

/**
 * Function iterates through a list of reserved windows for a link and returns
 * a free time slot for data upload
 *
 * @param occupancy_times List of occupied time slots for a link/edge
 * @param start_time The cutoff point, any free time slot before this we do not consider
 * @param data_size The size of the data to be transferred
 * @param bandwidth The bandwidth of the link
 * @return Valid time window for data upload
 */
std::pair<float, float>
NetworkTopologyServices::findLinkSlot(std::vector<std::pair<float, float>> occupancy_times, float start_time,
                                      float data_size, float bandwidth,
                                      float latency) {

    float duration = (data_size / bandwidth) + latency;

    pair<float, float> res = make_pair(start_time, start_time + duration);


    sort(occupancy_times.begin(), occupancy_times.end(),
         [](const pair<float, float> &a, const pair<float, float> &b) -> bool {
             return a.second < b.second;
         });

    for (int i = 0; i < occupancy_times.size(); i++) {
        if (occupancy_times[i].second < start_time) {
            if(i == occupancy_times.size() - 1) {
                res.first = start_time;
                res.second = res.first + duration;
            }
            continue;
        }

        if (i < occupancy_times.size() - 1) {
            float time_window = (occupancy_times[i + 1].first - 0.00001f) - occupancy_times[i].second + 0.00001f;
            if (duration <= time_window) {
                res.first = occupancy_times[i].second + 0.00001f;
                res.second = res.first + duration;
                break;
            }
        } else {
            res.first = occupancy_times[i].second + 0.00001f;
            res.second = res.first + duration;
            break;
        }
    }

    return res;
}

std::unordered_map<int, std::unordered_map<int, EdgePropertyData>>
NetworkTopologyServices::generateEdgeMap(NetworkTopology &network, int node_count) {
    property_map<NetworkTopology, edge_weight_t>::type weightMap = get(edge_weight, network);

    std::unordered_map<int, std::unordered_map<int, EdgePropertyData>> res;

    for (int i = 0; i < node_count; i++) {
        std::unordered_map<int, EdgePropertyData> tmp;

        for (int x = 0; x < node_count; x++) {
            if (x == i)
                continue;

            tmp.insert({x, get(weightMap, boost::edge((i < x) ? i : x, (i < x) ? x : i, network).first)});
        }

        res.insert({i, tmp});
    }

    return res;
}

NetworkTopology NetworkTopologyServices::generateNetwork() {
    ComputationNode cloudNodeA(INT_MAX, INT_MAX, INT_MAX, cloud);
    EdgeNode edgeNode(2 * SERVERS_PER_CLOUDLET, 64 * SERVERS_PER_CLOUDLET, 1000 * SERVERS_PER_CLOUDLET, node_type::edge, make_pair(1, 1));
    MobileNode mobileNode(1, 8, 128, mobile, make_pair(2, 2));
    EdgeNode edgeNodeB(2 * SERVERS_PER_CLOUDLET, 64 * SERVERS_PER_CLOUDLET, 1000 * SERVERS_PER_CLOUDLET, node_type::edge, make_pair(1, 1));
    EdgeNode edgeNodeC(2 * SERVERS_PER_CLOUDLET, 64 * SERVERS_PER_CLOUDLET, 1000 * SERVERS_PER_CLOUDLET, node_type::edge, make_pair(1, 1));

    NetworkTopology g;

    auto v1 = add_vertex({mobileNode.getType(), none, none, mobileNode}, g);
    auto v2 = add_vertex({edgeNode.getType(), none, edgeNode, none}, g);
    auto v3 = add_vertex({cloudNodeA.getType(), cloudNodeA, none, none}, g);
    auto v4 = add_vertex({edgeNodeB.getType(), none, edgeNodeB, none}, g);
    auto v5 = add_vertex({edgeNodeC.getType(), none, edgeNodeC, none}, g);

    add_edge(v1, v2, EdgePropertyData{(150.0f / 1000), 0.001 * 1000}, g);
    add_edge(v2, v1, EdgePropertyData{(150.0f / 1000), 0.001 * 1000}, g);

    add_edge(v1, v3, EdgePropertyData{(36.0f / 1000), 0.07 * 1000}, g);
    add_edge(v3, v1, EdgePropertyData{(36.0f / 1000), 0.07 * 1000}, g);

    add_edge(v1, v4, EdgePropertyData{(150.0f / 1000), 0.001 * 1000}, g);
    add_edge(v4, v1, EdgePropertyData{(150.0f / 1000), 0.001 * 1000}, g);

    add_edge(v1, v5, EdgePropertyData{(150.0f / 1000), 0.001 * 1000}, g);
    add_edge(v5, v1, EdgePropertyData{(150.0f / 1000), 0.001 * 1000}, g);

    add_edge(v2, v3, EdgePropertyData{(1024.0f / 1000), 0.02 * 1000}, g);
    add_edge(v3, v2, EdgePropertyData{(1024.0f / 1000), 0.02 * 1000}, g);

    add_edge(v2, v4, EdgePropertyData{(1024.0f / 1000), 0.01 * 1000}, g);
    add_edge(v4, v2, EdgePropertyData{(1024.0f / 1000), 0.01 * 1000}, g);

    add_edge(v2, v5, EdgePropertyData{(1024.0f / 1000), 0.01 * 1000}, g);
    add_edge(v5, v2, EdgePropertyData{(1024.0f / 1000), 0.01 * 1000}, g);

    add_edge(v3, v4, EdgePropertyData{(1024.0f / 1000), 0.02 * 1000}, g);
    add_edge(v4, v3, EdgePropertyData{(1024.0f / 1000), 0.02 * 1000}, g);

    add_edge(v3, v5, EdgePropertyData{(1024.0f / 1000), 0.02 * 1000}, g);
    add_edge(v5, v3, EdgePropertyData{(1024.0f / 1000), 0.02 * 1000}, g);

    add_edge(v4, v5, EdgePropertyData{(1024.0f / 1000), 0.01 * 1000}, g);
    add_edge(v5, v4, EdgePropertyData{(1024.0f / 1000), 0.01 * 1000}, g);

    return g;
}

vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex>
NetworkTopologyServices::getVertices(NetworkTopology &network) {
    vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> res;

    pair<network_vertex_iterator, network_vertex_iterator> vi = vertices(network);

    for (auto iter = vi.first; iter != vi.second; iter++) {
        res.push_back(network.m_vertices[*iter]);
    }

    return res;
}

SuperNode NetworkTopologyServices::getSuperNode(
        std::vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> networkVertexList) {
    std::unordered_map<string, float> res;

    SuperNode superNode;

    for (auto vert: networkVertexList) {
        ComputationNode &node = (vert.m_property.type == mobile) ? vert.m_property.mobileNode.get()
                                                                 : (vert.m_property.type ==
                                                                    cloud)
                                                                   ? vert.m_property.comp.get()
                                                                   : vert.m_property.edgeNode.get();

        superNode.addCapacity(node.getType());
        superNode.total_ram += node.getRam();
    }
    superNode.cloud_edge_gpu_cap = INT_MAX;
    return superNode;
}
