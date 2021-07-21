//
// Created by Jamie Cotter on 20/07/2021.
//

#include "main.h"
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <Models/ComputationNode/ComputationNode.h>
#include <Models/EdgeNode/EdgeNode.h>
#include <Models/MobileNode/MobileNode.h>

using namespace std;

typedef boost::property<boost::edge_weight_t, int> EdgeWeightProperty;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, EdgeWeightProperty> NetworkTopology;
typedef boost::graph_traits<NetworkTopology>::edge_iterator edge_iterator;
typedef boost::graph_traits<NetworkTopology>::vertex_iterator vertex_iterator;

int main() {
    NetworkTopology g;

    boost::add_edge(0, 1, 8, g);
    boost::add_edge(0, 3, 18, g);
    boost::add_edge(1, 2, 20, g);
    boost::add_edge(1, 3, 7, g);
    boost::add_edge(1, 4, 1, g);
    boost::add_edge(2, 3, 2, g);
    boost::add_edge(2, 5, 7, g);
    boost::add_edge(4, 5, 6, g);

    pair<edge_iterator, edge_iterator> ei = edges(g);

    pair<vertex_iterator, vertex_iterator> vi = vertices(g);


    cout << "Number of edges = " << num_edges(g) << "\n";
    cout << "Edge list:\n";

    boost::property_map<NetworkTopology, boost::edge_weight_t>:: type weightMap = get(boost::edge_weight, g);
    for(auto iter=ei.first; iter!=ei.second; iter++){
        cout << "edge " << *iter << " weight " << get(weightMap, *iter) << endl;
    }

    cout << endl << "Number of vertices = " << num_vertices(g) << "\n";
    cout << "Vertex list:\n";

    for(auto iter=vi.first; iter!=vi.second; iter++){
        cout << "vertex " << *iter << endl;
    }

    cout << endl;

    ComputationNode cloudNode(1, 10, 1, 1, cloud);
    cout << "Type " << cloudNode.getType() << endl;

    EdgeNode edgeNode(1, 10, 1, 1, edge, make_pair(1,1));
    cout << "Type " << edgeNode.getType() << endl;

    MobileNode mobileNode(1, 10, 1, 1, mobile, make_pair(2, 2));
    cout << "Type " << mobileNode.getType() << endl;

    return 0;
}