//
// Created by Jamie Cotter on 20/07/2021.
//

#include "main.h"
#include <Services/NetworkTopologyServices/NetworkTopologyServices.h>
#include <Services/ApplicationTopologyServices/ApplicationTopologyServices.h>
#include <Services/SimulatorServices/SimulatorFunctions.h>
#include <Services/FileReader/FileReader.h>
#include <fstream>

using namespace std;

int main(int argc, char *argv[]) {
    char *input_file_name = argv[1];
    char *output_file_name = argv[2];
    char *lower_bound_file_name = argv[3];

    float completion_time = 20.0f;

    NetworkTopology network = NetworkTopologyServices::generateNetwork();
    auto networkVertexList = NetworkTopologyServices::getVertices(network);

    int source_mobile_id = 0;

    for (auto &vertex: networkVertexList) {
        if (vertex.m_property.type == mobile) {
            source_mobile_id = vertex.m_property.mobileNode->getId();
            break;
        }
    }

    auto parsedApplications = FileReader::parseApplications(input_file_name, &completion_time);
    vector<ApplicationEvent> application_events;

    application_events.reserve(parsedApplications.size());

    for (auto &item: parsedApplications)
        application_events.push_back(
                {item.first, ApplicationTopologyServices::generateApplications(item, source_mobile_id)});
    try {
        SimulatorFunctions::programLoop(network, application_events, completion_time, output_file_name, lower_bound_file_name);
    }
    catch (const std::exception& e) {
        for(ApplicationEvent ae: application_events)
            ae.application.deleteTasks();
    }
    return 0;
}

void main::logLowerBoundTimes(vector<float> lower_bound_application_times, std::string output_file_name){
    ofstream myfile;
    myfile.open(output_file_name);

    for(float lb: lower_bound_application_times)
        myfile << lb << endl;

    myfile.close();
}

void main::logResults(const std::vector<TaskMapping> &finished, char *output_filename,
                      std::vector<ApplicationGraph>& total_task_lists) {
    ofstream myfile;
    myfile.open(output_filename);

    myfile << endl << "LOGGING MAPPING RESULTS:" << endl;
    for (const TaskMapping &i : finished) {
        Task finTask = *total_task_lists[i.taskId.first].taskList[i.taskId.second];
        myfile << "====================" << endl;
        myfile << "TASK: " << endl;
        myfile << finTask.to_string() << endl;
        myfile << "VERTEX:" << endl;

        if (i.node->type == cloud)
            myfile << i.node->comp->to_string();
        else if (i.node->type == node_type::edge)
            myfile << i.node->edgeNode->to_string();
        else if (i.node->type == mobile)
            myfile << i.node->mobileNode->to_string();

        myfile << endl << "START TIME: " << i.absoluteStart / 1000 << endl;
        myfile << "FINISH TIME: " << i.absoluteFinish / 1000 << endl;
        myfile << endl << "UPLOAD START TIME: " << i.uploadStart / 1000 << endl;
        myfile << "UPLOAD FINISH TIME: " << i.uploadFinish / 1000 << endl;
    }
    myfile.close();
    cout << output_filename;

    for(ApplicationGraph appGraph: total_task_lists)
        appGraph.deleteTasks();
}

/**
 * Function returns the time it takes for a node to process a task (currently basic)
 *
 * @param task - Task that we retrieve the MI count from
 * @param node - Node that we retrieves the MIPS count from
 * @return - Time derived from the millions of instructions / millions of instructions per second.
 */
float main::calculateProcessingTime(TaskVertexData task, NetworkVertexData node) {
    return task.task->getProcessTime(node.type);
}

/**
 * @param taskList - The list of tasks that represent all the nodes in a network
 * @return - A vector containing only the nodes not busy
 */
vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex>>
main::getReadyNodes(
        vector<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex> &networkList) {

    vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex>> res;
    for (auto &vertex : networkList) {
        if (vertex.m_property.type == mobile) {
            if (vertex.m_property.mobileNode->isFree())
                res.emplace_back(vertex);
        } else if (vertex.m_property.type == cloud) {
            if (vertex.m_property.comp->isFree())
                res.emplace_back(vertex);
        } else if (vertex.m_property.type == node_type::edge) {
            if (vertex.m_property.edgeNode->isFree())
                res.emplace_back(vertex);
        }
    }

    return res;
}

