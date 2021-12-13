//
// Created by Jamie Cotter on 22/07/2021.
//

#include "ApplicationTopologyServices.h"
#include <boost/graph/adjacency_list.hpp>
#include <Models/Task/Task.h>
#include <stack>
#include <Models/Mappings/SuperNode.h>

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
        Task generated_task = Task(strL[0], stof(strL[1]), stof(strL[2]), stof(strL[3]), stof(strL[4]), stoi(strL[5]) * 0.000001, stoi(strL[6]) * 0.000001,
                            stoi(strL[7]) * 0.000001, source_mobile_id, stoi(strL[8]));

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
        SuperNode super_node) {

//    vector<CriticalPathItem> critical_path_list;
//
//    for (int i = 0; i < taskList.size(); i++) {
//        vector<int> children;
//        vector<int> parents;
//        for (const auto &item: taskList[i].m_in_edges)
//            parents.push_back(item.m_target);
//
//        for (const auto &item: taskList[i].m_out_edges)
//            children.push_back(item.m_target);
//
//        critical_path_list.push_back({taskList[i].m_property.task.get().getId(), i, 0, 0, 0, 0, children, parents,
//                                      taskList[i].m_property.task.get().getMillionsOfInstructions()});
//    }
//
//    critical_path_list = EarliestMetricCalculator(critical_path_list);
//    critical_path_list = LatestMetricCalculator(critical_path_list);

    vector<TempTask*> finished_list;
    vector<TempTask*> inProgress;
    vector<TempTask> task_list;
    float time = 0.0f;

    for(const auto& task: taskList) {
        Task tempTask = task.m_property.task.get();

        std::vector<int> parents;
        for(auto m_in_edge: task.m_in_edges)
            parents.push_back((int) m_in_edge.m_target);

        std::vector<int> children;
        for(auto m_out_edge : task.m_out_edges)
            children.push_back((int) m_out_edge.m_target);

        task_list.push_back({parents, children, tempTask, false, false});
    }

    while(finished_list.size() != task_list.size()){
        if(!inProgress.empty()){
            inProgress[0]->finished = true;
            time = inProgress[0]->finish_time;
            finished_list.push_back(inProgress[0]);
            super_node.removeTask(*inProgress[0]);
            inProgress.erase(inProgress.begin());
        }

        for(int i = 0; i < task_list.size(); i++){
            if(task_list[i].offloaded || task_list[i].finished)
                continue;

            bool ready = true;
            for(int index: task_list[i].parents){
                if(!task_list[index].finished) {
                    ready = false;
                    break;
                }
            }

            if(ready && super_node.total_ram_usage + task_list[i].task.getRam() <= super_node.total_ram){
                bool add = false;
                if(super_node.cloud_edge_usage + 1 <= super_node.cloud_edge_gpu_cap && task_list[i].task.isOffload()){
                    task_list[i].allocated_node = node_type::cloud;
                    add = true;
                }
                else if (super_node.mobile_usage + 1 <= super_node.mobile_gpu_cap && !task_list[i].task.isOffload()){
                    task_list[i].allocated_node = node_type::mobile;
                    add = true;
                }
                if(add){
                    task_list[i].finish_time = time + task_list[i].task.getProcessTime(task_list[i].allocated_node);
                    super_node.addTask(task_list[i]);
                    int index = 0;
                    for(;index < inProgress.size() && task_list[i].finish_time < inProgress[index]->finish_time; index++);
                    task_list[i].offloaded = true;
                    inProgress.insert(inProgress.begin() + index, &task_list[i]);
                }
            }
        }

    }
    return time;
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
