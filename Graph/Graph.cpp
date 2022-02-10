//
// Created by Jamie Cotter on 21/01/2022.
//

#include "Graph.h"
#include "../Model/ComputationNode/ComputationNode.h"
#include "EdgeData/EdgeData.h"
#include "../Model/Task/Task.h"

template<typename T, typename J>
void Graph<T, J>::addVertex(T *node) {
    std::shared_ptr<T> sharedPtr(node);
    this->vertices.emplace(node->getId(), sharedPtr);
}

template<typename T, typename J>
void Graph<T, J>::addEdge(int source, int destination, J edgeData) {

    if (this->out_edges.count(source) == 0)
        this->out_edges.emplace(source, std::unordered_map<int, J>());

    if (this->in_edges.count(destination) == 0)
        this->in_edges.emplace(destination, std::unordered_map<int, J>());

    this->out_edges.at(source).emplace(destination, edgeData);
    this->in_edges.at(destination).emplace(source, edgeData);

    if (!is_directed) {
        if (this->in_edges.count(source) == 0)
            this->in_edges.emplace(source, std::unordered_map<int, J>());

        if (this->out_edges.count(destination) == 0)
            this->out_edges.emplace(destination, std::unordered_map<int, J>());

        this->in_edges.at(source).emplace(destination, edgeData);
        this->out_edges.at(destination).emplace(source, edgeData);
    }
}

template<typename T, typename J>
Graph<T, J>::Graph(bool isDirected):is_directed(isDirected) {}

template<typename T, typename J>
std::vector<int> Graph<T, J>::getInEdgesID(int source_node_id) {
    std::vector<int> res;
    if (this->in_edges.count(source_node_id)) {
        std::unordered_map<int, J> edge_table = in_edges.at(source_node_id);
        for (auto const&[key, val]: edge_table)
            res.push_back(key);
    }
    return res;
}

template<typename T, typename J>
std::vector<J> Graph<T, J>::getInEdgesObj(int source_node_id) {
    std::vector<J> res;
    if (this->in_edges.count(source_node_id)) {
        std::unordered_map<int, J> edge_table = in_edges.at(source_node_id);
        for (auto const&[key, val]: edge_table)
            res.push_back(val);
    }
    return res;
}

template<typename T, typename J>
std::vector<std::shared_ptr<T>> Graph<T, J>::getOutEdgesVertex(int source_node_id) {
    std::vector<std::shared_ptr<T>> res;
    if (this->out_edges.count(source_node_id)) {
        std::unordered_map<int, J> edge_table = out_edges.at(source_node_id);
        for (auto const&[key, val]: edge_table)
            res.push_back(this->vertices.at(key));
    }
    return res;
}


template
class Graph<ComputationNode, std::shared_ptr<EdgeData>>;

template
class Graph<Task, bool>;
