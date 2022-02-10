//
// Created by Jamie Cotter on 21/01/2022.
//

#ifndef UNTITLED2_GRAPH_H
#define UNTITLED2_GRAPH_H

#include <unordered_map>

template<typename T, typename J>
class Graph {
private:
    bool is_directed;

public:
    explicit Graph(bool isDirected);

    std::unordered_map<int, std::shared_ptr<T>> vertices;
    std::unordered_map<int, std::unordered_map<int, J>> in_edges;
    std::unordered_map<int, std::unordered_map<int, J>> out_edges;

    void addVertex(T *node);

    void addEdge(int source, int destination, J edgeData);

    std::vector<int> getInEdgesID(int source_node_id);

    std::vector<J> getInEdgesObj(int source_node_id);

    std::vector<std::shared_ptr<T>> getOutEdgesVertex(int source_node_id);
};

#endif //UNTITLED2_GRAPH_H
