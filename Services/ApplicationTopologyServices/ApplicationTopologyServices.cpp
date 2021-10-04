//
// Created by Jamie Cotter on 22/07/2021.
//

#include "ApplicationTopologyServices.h"
#include <boost/graph/adjacency_list.hpp>
#include <Models/Task/Task.h>
#include <stack>

using namespace std;

struct CriticalPathItem {
    int id;
    int index;

    float est;
    float lst;
    float eet;
    float let;
    vector<int> children;
    vector<int> parents;

    unsigned long long MI;
};

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

void topologicalSort(int curr, vector<bool> visited, stack<int> &sorted_topology, vector<vector<int>> adjacency) {
    visited[curr] = true;

    for (int i = 0; i < adjacency[curr].size(); ++i)
        if (!visited[i]) topologicalSort(i, visited, sorted_topology, adjacency);

    sorted_topology.push(curr);
}

vector<CriticalPathItem> EarliestMetricCalculator(vector<CriticalPathItem> critical_path_items) {
    critical_path_items[0].eet = critical_path_items[0].eet + critical_path_items[0].MI;

    for (int i = 1; i < critical_path_items.size(); i++) {
        for (int cpm_item: critical_path_items[i].parents) {
            if (critical_path_items[i].est < critical_path_items[cpm_item].eet)
                critical_path_items[i].est = critical_path_items[cpm_item].eet;
        }

        critical_path_items[i].eet = critical_path_items[i].est + critical_path_items[i].MI;
    }

    return critical_path_items;
}

vector<CriticalPathItem> LatestMetricCalculator(vector<CriticalPathItem> critical_path_items) {

    critical_path_items[critical_path_items.size() - 1].let = critical_path_items[critical_path_items.size() - 1].eet;
    critical_path_items[critical_path_items.size() - 1].lst = critical_path_items[critical_path_items.size() - 1].let -
                                                              critical_path_items[critical_path_items.size() - 1].MI;

    for (int i = critical_path_items.size() - 2; i >= 0; i--) {
        for (int cpm_item: critical_path_items[i].children) {
            if (critical_path_items[i].let == 0)
                critical_path_items[i].let = critical_path_items[cpm_item].lst;
            else if (critical_path_items[i].let > critical_path_items[cpm_item].lst)
                critical_path_items[i].let = critical_path_items[cpm_item].lst;
        }

        critical_path_items[i].lst = critical_path_items[i].let - critical_path_items[i].MI;
    }

    return critical_path_items;
}

float total_path_time(vector<CriticalPathItem> critical_path_list) {
    int total_path_MI = 0;
    for (CriticalPathItem cp_item: critical_path_list) {
        if ((cp_item.eet - cp_item.let == 0) && (cp_item.est - cp_item.lst == 0))
            total_path_MI += cp_item.MI;
    }

    return total_path_MI;
}

float ApplicationTopologyServices::calculateLowerBound(
        vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex> &taskList,
        int super_node_MI) {

    vector<CriticalPathItem> critical_path_list;

    for (int i = 0; i < taskList.size(); i++) {
        vector<int> children;
        vector<int> parents;
        for (const auto &item: taskList[i].m_in_edges)
            parents.push_back(item.m_target);

        for (const auto &item: taskList[i].m_out_edges)
            children.push_back(item.m_target);

        critical_path_list.push_back({taskList[i].m_property.task.get().getId(), i, 0, 0, 0, 0, children, parents,
                                      taskList[i].m_property.task.get().getMillionsOfInstructions()});
    }

    critical_path_list = EarliestMetricCalculator(critical_path_list);
    critical_path_list = LatestMetricCalculator(critical_path_list);
    float total_time = critical_path_list[critical_path_list.size() - 1].eet / super_node_MI;
    return total_time;
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
