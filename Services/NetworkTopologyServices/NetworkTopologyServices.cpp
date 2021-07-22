//
// Created by Jamie Cotter on 22/07/2021.
//

#include "NetworkTopologyServices.h"
#include <boost/graph/adjacency_list.hpp>
#include <Models/ComputationNode/ComputationNode.h>
#include <Models/EdgeNode/EdgeNode.h>
#include <Models/MobileNode/MobileNode.h>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <Models/VertexData/NetworkVertexData.h>

using namespace std;

/**
 * This function will find the shortest path from a source node to every other node within the graph.
 *
 * @param start - An integer representing the node you want to find the shortest path to all other nodes from
 * @param network - A reference to a graph topology that you want to navigate
 *
 * @return - A pair that contains:
 *  [0] <vector<unsigned long>> A vector that contains the shortest path to each node
 *  [1] A list that contains the distance to each node
 *
 */
pair<vector<unsigned long>, iterator_property_map<__wrap_iter<int *>, vec_adj_list_vertex_id_map<NetworkVertexData, unsigned long>, int, int &>>
NetworkTopologyServices::shortestPaths(int start, NetworkTopology &network) {
    vector<network_vertex_descriptor> pred(num_vertices(network));
    iterator_property_map<vector<network_vertex_descriptor>::iterator,
            IdMap,
            network_vertex_descriptor,
            network_vertex_descriptor &>
            pred_map(pred.begin(), get(vertex_index, network));

    std::vector<int> dist_vector(num_vertices(network));
    boost::iterator_property_map<std::vector<int>::iterator,
            IdMap,
            int,
            int &>
            distmap_vect(dist_vector.begin(), get(boost::vertex_index, network));

    dijkstra_shortest_paths(network, 0, predecessor_map(pred_map).distance_map(distmap_vect));

    return make_pair(pred, distmap_vect);
}

/**
 * Will provide a list of ints describing the route between two points.
 *
 * @param start - The index of the starting node
 * @param goal - The index of the end node
 * @param pred - The list of vectors contain the shortest path to each node
 *
 * @return - A vector of ints representing the nodes you must traverse to travel from start -> goal
 */
std::vector<int> NetworkTopologyServices::shortestRouteBetweenTwoPoints(int start, int goal, vector<network_vertex_descriptor> pred) {
    int current = goal;
    vector<int> route;
    do {
        if (pred[current] != start)
            route.insert(route.begin(), pred[current]);
        current = pred[current];
    } while (current != start);

    return route;
};

void NetworkTopologyServices::logInfo(NetworkTopology &network) {
    pair<network_edge_iterator, network_edge_iterator> ei = edges(network);

    pair<network_vertex_iterator, network_vertex_iterator> vi = vertices(network);

    cout << "Number of edges = " << num_edges(network) << "\n";
    cout << "Edge list:\n";

    property_map<NetworkTopology, edge_weight_t>::type weightMap = get(edge_weight, network);
    for (auto iter = ei.first; iter != ei.second; iter++) {
        cout << "edge " << *iter << " weight " << INT_MAX - get(weightMap, *iter) << endl;
    }

    cout << endl << "Number of vertices = " << num_vertices(network) << "\n";
    cout << "Vertex list:\n" << endl;

    for (auto iter = vi.first; iter != vi.second; iter++) {
        node_type type = get(&NetworkVertexData::type, network)[(int) *iter];
        if (type == mobile)
            cout << "Vertex " << *iter << ":\n" << get(&NetworkVertexData::mobileNode, network)[*iter].get().to_string()
                 << endl;
        else if (type == node_type::edge)
            cout << "Vertex " << *iter << ":\n" << get(&NetworkVertexData::edgeNode, network)[*iter].get().to_string()
                 << endl;
        else if (type == cloud)
            cout << "Vertex " << *iter << ":\n" << get(&NetworkVertexData::comp, network)[*iter].get().to_string()
                 << endl;
    }

    int start = 0;
    int goal = 4;

    auto paths_and_dist = NetworkTopologyServices::shortestPaths(start, network);
    vector<int> route = NetworkTopologyServices::shortestRouteBetweenTwoPoints(start, goal, paths_and_dist.first);

    cout << "Shortest route between vertices " << start << " and " << goal << ":" << endl;

    for (std::vector<int>::iterator it = route.begin(); it != route.end(); ++it) {
        cout << " Vertex: " << *it << endl;
    }
}

NetworkTopology NetworkTopologyServices::generateNetwork() {
    ComputationNode cloudNode(1, 15, 128, 1000, cloud);
    cout << "Type " << cloudNode.printType() << endl;

    EdgeNode edgeNode(1, 10, 8, 250, node_type::edge, make_pair(1, 1));
    cout << "Type " << edgeNode.printType() << endl;

    MobileNode mobileNode(1, 4, 8, 16, mobile, make_pair(2, 2));
    cout << "Type " << mobileNode.printType() << endl << endl;

    NetworkTopology g;

    auto v1 = add_vertex({mobileNode.getType(), none, none, mobileNode}, g);
    auto v2 = add_vertex({edgeNode.getType(), none, edgeNode, none}, g);
    auto v3 = add_vertex({cloudNode.getType(), cloudNode, none, none}, g);
    auto v4 = add_vertex({cloudNode.getType(), cloudNode, none, none}, g);
    auto v5 = add_vertex({cloudNode.getType(), cloudNode, none, none}, g);

    add_edge(v1, v2, INT_MAX - 5, g);
    add_edge(v1, v3, INT_MAX - 5, g);
    add_edge(v1, v4, INT_MAX - 10, g);
    add_edge(v1, v5, INT_MAX - 10, g);
    add_edge(v2, v3, INT_MAX - 6, g);
    add_edge(v2, v4, INT_MAX - 6, g);
    add_edge(v2, v5, INT_MAX - 6, g);
    add_edge(v3, v4, INT_MAX - 5, g);
    add_edge(v3, v5, INT_MAX - 5, g);
    add_edge(v4, v5, INT_MAX - 5, g);


    return g;
}
