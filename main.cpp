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

    SimulatorFunctions::programLoop(network, application_events, completion_time, output_file_name);

    return 0;
}

void main::programLoop(NetworkTopology &network, ApplicationTopology &navigator) {
    //Retrieving the list of vertices and their edges(in & out) in both our generated application and our network
    auto navigatorVertexList = ApplicationTopologyServices::getVertices(navigator);
    auto networkVertexList = NetworkTopologyServices::getVertices(network);

    //Will hold the tasks ready to be offloaded in each loop
    vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>>, vecS, vecS, bidirectionalS, TaskVertexData, property<edge_weight_t, int>, no_property, listS>::config::stored_vertex>>
            readyTaskList;

    //Will hold the nodes ready to be offloaded in each loop
    vector<std::reference_wrapper<detail::adj_list_gen<adjacency_list<vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>>, vecS, vecS, bidirectionalS, NetworkVertexData, property<edge_weight_t, EdgePropertyData>, no_property, listS>::config::stored_vertex>>
            readyNodeList;

    //Contains a list of tasks mapped to nodes currently computing
    vector<TaskMapping> inProgress;

    //Contains a finished list of tasks
    vector<TaskMapping> finished;

    //Represents time in seconds
    float time = 0.0f;

    while (finished.size() != navigatorVertexList.size()) {
        //Sorting the current event list from most recent to oldest time
        inProgress = SimulatorFunctions::sortEventList(inProgress);

        //Moving to the next event
        if (!inProgress.empty()) {
            TaskMapping tM = inProgress.back();
            inProgress.pop_back();

            time = tM.absoluteFinish;
            tM.task.get().task.get().setDone(true);
            tM.task.get().task.get().setInProgress(false);

            if (tM.node.get().type == mobile)
                tM.node.get().mobileNode->setIsFree(true);
            else if (tM.node.get().type == cloud)
                tM.node.get().comp->setIsFree(true);
            else if (tM.node.get().type == node_type::edge)
                tM.node.get().edgeNode->setIsFree(true);

            finished.push_back(tM);
        }

        readyTaskList = SimulatorFunctions::getReadyTasks(navigatorVertexList);
        readyNodeList = main::getReadyNodes(networkVertexList);

        int count = (readyTaskList.size() < readyNodeList.size()) ? readyTaskList.size() : readyNodeList.size();

        for (int i = 0; i < count; i++) {
            auto &selectedTask = readyTaskList.back().get().m_property;
            readyTaskList.pop_back();

            auto &selectedNode = readyNodeList.back().get().m_property;
            readyNodeList.pop_back();

            selectedTask.task.get().setInProgress(true);

            if (selectedNode.type == mobile)
                selectedNode.mobileNode->setIsFree(false);
            else if (selectedNode.type == cloud) {
                selectedNode.comp->setIsFree(false);
            } else if (selectedNode.type == node_type::edge)
                selectedNode.edgeNode->setIsFree(false);

            float finish = time + main::calculateProcessingTime(selectedTask, selectedNode);
            inProgress.push_back({time, finish, selectedTask, selectedNode});
        }

    }

    main::logResults(finished, (char* ) "output_file.txt");
}

void main::logResults(const vector<TaskMapping> &finished, char* output_filename) {
    ofstream myfile;
    myfile.open(output_filename);

    myfile << endl << "LOGGING MAPPING RESULTS:" << endl;
    for (const auto &i : finished) {
        myfile << "====================" << endl;
        myfile << "TASK: " << endl;
        myfile << i.task.get().task->to_string() << endl;
        myfile << "VERTEX:" << endl;

        if (i.node.get().type == cloud)
            myfile << i.node.get().comp->to_string();
        else if (i.node.get().type == node_type::edge)
            myfile << i.node.get().edgeNode->to_string();
        else if (i.node.get().type == mobile)
            myfile << i.node.get().mobileNode->to_string();

        myfile << endl << "START TIME: " << i.processStartTime << endl;
        myfile << "FINISH TIME: " << i.processFinishTime << endl;
        myfile << endl << "UPLOAD START TIME: " << i.uploadStart << endl;
        myfile << "UPLOAD FINISH TIME: " << i.uploadFinish << endl;
    }
    myfile.close();
    cout << output_filename;
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

