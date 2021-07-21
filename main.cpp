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
typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, boost::no_property, EdgeWeightProperty> DirectedGraph;
typedef boost::graph_traits<DirectedGraph>::edge_iterator edge_iterator;
typedef boost::graph_traits<DirectedGraph>::vertex_iterator vertex_iterator;

int main() {
    DirectedGraph g;

    boost::add_edge(0, 1, 8, g);
    boost::add_edge(0, 3, 18, g);
    boost::add_edge(1, 2, 20, g);
    boost::add_edge(2, 3, 2, g);
    boost::add_edge(3, 1, 1, g);
    boost::add_edge(1, 3, 7, g);
    boost::add_edge(1, 4, 1, g);
    boost::add_edge(4, 5, 6, g);
    boost::add_edge(2, 5, 7, g);

    pair<edge_iterator, edge_iterator> ei = edges(g);


    cout << "Number of edges = " << num_edges(g) << "\n";
    cout << "Edge list:\n";

    copy(ei.first, ei.second,
         ostream_iterator<boost::adjacency_list<>::edge_descriptor>{
                 cout, "\n"
         });

    cout << endl;

    ComputationNode cloudNode(1, 10, 1, 1, cloud);
    cout << "Type " << cloudNode.getType() << endl;

    EdgeNode edgeNode(1, 10, 1, 1, edge, 0, 0);
    cout << "Type " << edgeNode.getType() << endl;

    MobileNode mobileNode(1, 10, 1, 1, mobile, 0, 0);
    cout << "Type " << mobileNode.getType() << endl;

    return 0;
}