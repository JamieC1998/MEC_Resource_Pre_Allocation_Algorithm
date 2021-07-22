//
// Created by Jamie Cotter on 20/07/2021.
//

#include "main.h"
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <Models/ComputationNode/ComputationNode.h>
#include <Models/EdgeNode/EdgeNode.h>
#include <Models/MobileNode/MobileNode.h>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <Models/NetworkVertexData.h>

using namespace std;

int main() {
    NetworkTopology network = main::generateNetwork();
    main::logInfo(network);

    int start = 0;
    int goal = 4;
    vector<int> route = main::shortestRouteBetweenTwoPoints(start, goal, network);
    cout << "Shortest route between vertices " << start << " and " << goal << ":" << endl;
    for(std::vector<int>::iterator it = route.begin(); it != route.end(); ++it) {
        cout << " Vertex: " << *it << endl;
    }

    return 0;
}

std::vector<int> main::shortestRouteBetweenTwoPoints(int start, int goal, NetworkTopology& network){
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
            int&>
            distmap_vect(dist_vector.begin(), get(boost::vertex_index, network));

    dijkstra_shortest_paths(network, 0, predecessor_map(pred_map).distance_map(distmap_vect));

    int current = goal;
    vector<int> route;
    do{
        if(pred[current] != start)
            route.insert(route.begin(), pred[current]);
        current = pred[current];
    }while(current != start);

    return route;
};

void main::logInfo(NetworkTopology &network) {
    pair<network_edge_iterator, network_edge_iterator> ei = edges(network);

    pair<network_vertex_iterator, network_vertex_iterator> vi = vertices(network);

    cout << "Number of edges = " << num_edges(network) << "\n";
    cout << "Edge list:\n";

    property_map<NetworkTopology, edge_weight_t>::type weightMap = get(edge_weight, network);
    for (auto iter = ei.first; iter != ei.second; iter++) {
        cout << "edge " << *iter << " weight " << get(weightMap, *iter) << endl;
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
}

NetworkTopology main::generateNetwork() {
    ComputationNode cloudNode(1, 10, 1, 1, cloud);
    cout << "Type " << cloudNode.getType() << endl;

    EdgeNode edgeNode(1, 10, 1, 1, node_type::edge, make_pair(1, 1));
    cout << "Type " << edgeNode.getType() << endl;

    MobileNode mobileNode(1, 10, 1, 1, mobile, make_pair(2, 2));
    cout << "Type " << mobileNode.getType() << endl << endl;

    NetworkTopology g;

    auto v1 = add_vertex({mobileNode.getType(), none, none, mobileNode}, g);
    auto v2 = add_vertex({edgeNode.getType(), none, edgeNode, none}, g);
    auto v3 = add_vertex({cloudNode.getType(), cloudNode, none, none}, g);
    auto v4 = add_vertex({cloudNode.getType(), cloudNode, none, none}, g);
    auto v5 = add_vertex({cloudNode.getType(), cloudNode, none, none}, g);

    add_edge(v1, v2, 5, g);
    add_edge(v1, v3, 5, g);
    add_edge(v2, v4, 15, g);
    add_edge(v3, v4, 5, g);
    add_edge(v4, v5, 5, g);


    return g;
}