//
// Created by Jamie Cotter on 22/07/2021.
//

#include "ApplicationTopologyServices.h"
#include <boost/graph/adjacency_list.hpp>
#include <Models/Task/Task.h>

using namespace std;

ApplicationTopology ApplicationTopologyServices::generateApplications(
        std::pair<float, std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>> item,
        int source_mobile_id) {

    ApplicationTopology a;

    for (int j = 0; j < item.second.size(); j++) {
        auto &application = item.second[j];
        vector<string> strL = application.first;
        vector<string> edgeL = application.second;
        Task generated_task = Task(strL[0], stof(strL[1]) * 1000, stof(strL[2]) * 1000, stof(strL[3]) * 1000, stof(strL[4]), stof(strL[5]), stof(strL[6]),
                                   stof(strL[7]), source_mobile_id, stoi(strL[8]));

        add_vertex({generated_task}, a);

        for (auto &i : edgeL)
            add_edge(stoi(i), j, 1, a);

    }

    return a;
}

vector<float> ApplicationTopologyServices::getParentData(vector<vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>& total_task_lists, int source_node_id){
    int application = -1;
    int index = -1;

    vector<float> data_in;

    for(int i = 0; i < total_task_lists.size(); i++){
        for(int x = 0; x < total_task_lists[i].size(); x++){
            if(total_task_lists[i][x].m_property.task->getId() == source_node_id){
                application = i;
                index = x;
                break;
            }
        }
        if(application != -1)
            break;
    }

    if(application != -1){
        for(int i = 0; i < total_task_lists[application][index].m_in_edges.size(); i++){
            data_in.push_back(total_task_lists[application][total_task_lists[application][index].m_in_edges[i].m_target].m_property.task->getDataOut());
        }
    }

    return data_in;
}

vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>
ApplicationTopologyServices::getVertices(ApplicationTopology &application) {
    vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> res;

    pair<application_vertex_iterator, application_vertex_iterator> vi = vertices(application);

    for (auto iter = vi.first; iter != vi.second; iter++) {
        res.push_back(application.m_vertices[*iter]);
    }

    return res;
}


void ApplicationTopologyServices::logInfo(ApplicationTopology &application) {
    pair<application_edge_iterator, application_edge_iterator> ei = edges(application);

    pair<application_vertex_iterator, application_vertex_iterator> vi = vertices(application);

    cout << "Number of edges = " << num_edges(application) << "\n";
    cout << "Edge list:\n";

    property_map<ApplicationTopology, edge_weight_t>::type weightMap = get(edge_weight, application);
    for (auto iter = ei.first; iter != ei.second; iter++) {
        cout << "edge " << *iter << " weight " << get(weightMap, *iter) << endl;
    }

    cout << endl << "Number of vertices = " << num_vertices(application) << "\n";
    cout << "Vertex list:\n" << endl;

    for (auto iter = vi.first; iter != vi.second; iter++) {
        cout << "Vertex " << *iter << ":\n" << get(&TaskVertexData::task, application)[*iter]->to_string()
             << " Out Edge count: " << application.m_vertices[*iter].m_out_edges.size() << endl
             << endl;
    }

}
