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
        Task generated_task(strL[0], stoi(strL[1]), stof(strL[2]), stoi(strL[3]), stoi(strL[4]),
                            stoi(strL[5]), source_mobile_id, stoi(strL[6]), stoi(strL[7]));

        add_vertex({generated_task}, a);

        for (auto &i : edgeL)
            add_edge(stoi(i), j, 1, a);

    }

    return a;
}

ApplicationTopology ApplicationTopologyServices::generateNavigator(int source_mobile_id) {
    Task rootTask("CONF_PANEL", 1, 1, 5, 5, 1, source_mobile_id, false, 1);
    Task gpsTask("GPS", 1, 3, 5, 5, 5, source_mobile_id, true, 1);
    Task controlTask("CONTROL", 2, 3, 5, 5, 1, source_mobile_id, true, 1);
    Task mapsTask("MAPS", 3, 5, 50, 50, 5, source_mobile_id, true, 1);
    Task pathCalcTask("PATH_CALC", 5, 2, 50, 50, 5, source_mobile_id, true, 1);
    Task trafficTask("TRAFFIC", 5, 1, 50, 50, 5, source_mobile_id, true, 1);
    Task voiceSynthTask("VOICE_SYNTH", 2, 1, 5, 20, 5, source_mobile_id, true, 1);
    Task guiTask("GUI", 2, 1, 5, 20, 5, source_mobile_id, true, 1);
    Task speedTrapTask("SPEED_TRAP", 2, 1, 10, 10, 5, source_mobile_id, true, 1);

    ApplicationTopology a;

    auto v1 = add_vertex({rootTask}, a);
    auto v2 = add_vertex({gpsTask}, a);
    auto v3 = add_vertex({controlTask}, a);
    auto v4 = add_vertex({mapsTask}, a);
    auto v5 = add_vertex({pathCalcTask}, a);
    auto v6 = add_vertex({trafficTask}, a);
    auto v7 = add_vertex({voiceSynthTask}, a);
    auto v8 = add_vertex({guiTask}, a);
    auto v9 = add_vertex({speedTrapTask}, a);

    auto e1 = add_edge(v1, v3, 1, a);
    auto e2 = add_edge(v2, v3, 1, a);
    auto e3 = add_edge(v3, v4, 1, a);
    auto e4 = add_edge(v3, v5, 1, a);
    auto e5 = add_edge(v3, v6, 1, a);
    auto e6 = add_edge(v4, v5, 1, a);
    auto e7 = add_edge(v6, v5, 1, a);
    auto e8 = add_edge(v5, v7, 1, a);
    auto e9 = add_edge(v5, v8, 1, a);
    auto e10 = add_edge(v5, v9, 1, a);

    return a;
}

ApplicationTopology ApplicationTopologyServices::generateChessApp(int source_mobile_id) {
    Task guiTask("GUI", 4, 1, 5, 5, 1, source_mobile_id, false, 1);
    Task updateChessTask("UPDATE_CHESS", 2, 1, 5, 5, 1, source_mobile_id, true, 1);
    Task computeMovesTask("COMPUTE_MOVES", 2, 1, 5, 5, 1, source_mobile_id, true, 4);
    Task outputTask("OUTPUT_TASK", 2, 1, 5, 5, 1, source_mobile_id, false, 1);

    ApplicationTopology a;

    auto v1 = add_vertex({guiTask}, a);
    auto v2 = add_vertex({updateChessTask}, a);
    auto v3 = add_vertex({computeMovesTask}, a);
    auto v4 = add_vertex({outputTask}, a);

    auto e1 = add_edge(v1, v2, 1, a);
    auto e2 = add_edge(v2, v3, 1, a);
    auto e3 = add_edge(v3, v4, 1, a);

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
