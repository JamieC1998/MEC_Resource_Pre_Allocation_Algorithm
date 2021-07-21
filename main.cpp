//
// Created by Jamie Cotter on 20/07/2021.
//

#include "main.h"
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <Models/ComputationNode/ComputationNode.h>
#include <Models/EdgeNode/EdgeNode.h>
#include <Models/MobileNode/MobileNode.h>
#include <Models/NetworkVertexData.h>

using namespace std;

int main() {
    NetworkTopology network = main::generateNetwork();

    main::logInfo(network);

    return 0;
}

void main::logInfo(NetworkTopology &network) {
    pair<edge_iterator, edge_iterator> ei = edges(network);

    pair<vertex_iterator, vertex_iterator> vi = vertices(network);

    cout << "Number of edges = " << num_edges(network) << "\n";
    cout << "Edge list:\n";

    boost::property_map<NetworkTopology, boost::edge_weight_t>::type weightMap = get(boost::edge_weight, network);
    for (auto iter = ei.first; iter != ei.second; iter++) {
        cout << "edge " << *iter << " weight " << get(weightMap, *iter) << endl;
    }

    cout << endl << "Number of vertices = " << num_vertices(network) << "\n";
    cout << "Vertex list:\n";

    for (auto iter = vi.first; iter != vi.second; iter++) {
        node_type type = get(&NetworkVertexData::type, network)[(int) *iter];
        if (type == mobile)
            cout << "Vertex " << *iter << ":\n" << get(&NetworkVertexData::mobileNode, network)[*iter].get().to_string() << endl;
    }
}

NetworkTopology main::generateNetwork() {
    ComputationNode cloudNode(1, 10, 1, 1, cloud);
    cout << "Type " << cloudNode.getType() << endl;

    EdgeNode edgeNode(1, 10, 1, 1, edge, make_pair(1, 1));
    cout << "Type " << edgeNode.getType() << endl;

    MobileNode mobileNode(1, 10, 1, 1, mobile, make_pair(2, 2));
    cout << "Type " << mobileNode.getType() << endl << endl;

    NetworkTopology g;

    auto v1 = boost::add_vertex({mobileNode.getType(), boost::none, boost::none, mobileNode}, g);
    auto v2 = boost::add_vertex({edgeNode.getType(), boost::none, edgeNode, boost::none}, g);
    auto v3 = boost::add_vertex({cloudNode.getType(), cloudNode, boost::none, boost::none}, g);

    boost::add_edge(v1, v2, 8, g);
    boost::add_edge(v2, v3, 7, g);

    return g;
}