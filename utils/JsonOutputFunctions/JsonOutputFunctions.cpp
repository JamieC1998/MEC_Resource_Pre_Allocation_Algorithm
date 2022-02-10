//
// Created by Jamie Cotter on 07/02/2022.
//

#include "JsonOutputFunctions.h"
#include "../HelperFunctions/HelperFunctions.h"
#include <fstream>

void JsonOutputFunctions::outputJson(std::vector<std::shared_ptr<TaskMapping>> &finished,
                                     std::vector<std::shared_ptr<Graph<Task, bool>>> &processing_applications,
                                     std::vector<std::pair<float, std::shared_ptr<Graph<Task, bool>>>> &unprocessed_applications,
                                     const char *output_file_json,
                                     Graph<ComputationNode, std::shared_ptr<EdgeData>> &nG) {
    std::ofstream json_file_stream;
    json_file_stream.open(output_file_json);

    json_file_stream << "{" << std::endl;

    json_file_stream << "\t\"application_count\": " << unprocessed_applications.size() + processing_applications.size()
                     << "," << std::endl;
    json_file_stream << "\t\"completed_application_count\": "
                     << HelperFunctions::countApplicationsCompleted(processing_applications) << "," << std::endl;

    json_file_stream << "\t\"total_task_count\": "
                     << JsonOutputFunctions::totalTaskCount(processing_applications, unprocessed_applications) << ","
                     << std::endl;

    json_file_stream << "\t\"finish_time\": " << finished.back()->getFinishValue() << ","
                     << std::endl;

    json_file_stream << "\t\"completed_task_count\": " << finished.size() << "," << std::endl;

    json_file_stream << "\t\"completed_tasks\": [" << std::endl;

    JsonOutputFunctions::outputTaskData(finished, json_file_stream, nG);

    json_file_stream << "\t]," << std::endl;

    json_file_stream << "\t\"edge_upload_windows\": {" << std::endl;

    JsonOutputFunctions::edgeUploadWindows(nG, json_file_stream);

    json_file_stream << "\t}," << std::endl;

    json_file_stream << "\t\"node_types\": {" << std::endl;

    for(int i = 0; i < nG.vertices.size(); i++){
        json_file_stream << "\t\t\"" << nG.vertices[i]->getId() << "\": " << "\"" << nG.vertices[i]->printType() << "\"";

        if (i < nG.vertices.size() - 1)
            json_file_stream << "," << std::endl;
        else
            json_file_stream << std::endl;
    }

    json_file_stream << "\t}" << std::endl;

    json_file_stream << "}" << std::endl;


}

int JsonOutputFunctions::totalTaskCount(std::vector<std::shared_ptr<Graph<Task, bool>>> &processing_apps,
                                        std::vector<std::pair<float, std::shared_ptr<Graph<Task, bool>>>> &unprocessed_apps) {
    int total_task_count = 0;

    for (const auto &app: processing_apps)
        total_task_count += (int) app->vertices.size();

    for (const auto &app: unprocessed_apps)
        total_task_count += (int) app.second->vertices.size();

    return total_task_count;
}

void JsonOutputFunctions::outputTaskData(std::vector<std::shared_ptr<TaskMapping>> &finished_task_list,
                                         std::ofstream &ofstream,
                                         Graph<struct ComputationNode, std::shared_ptr<EdgeData>> &network) {

    for (int i = 0; i < finished_task_list.size(); i++) {
        ofstream << "\t\t{" << std::endl;

        ofstream << "\t\t\t" << "\"task_name\": " << "\"" << finished_task_list[i]->getTask()->getName() << "\"" << ","
                 << std::endl;
        ofstream << "\t\t\t" << "\"total_task_id\": " << finished_task_list[i]->getTask()->getTotalTaskId() << ","
        << std::endl;
        ofstream << "\t\t\t" << "\"application_id\": " << finished_task_list[i]->getTask()->getApplicationId() << ","
        << std::endl;
        ofstream << "\t\t\t" << "\"task_id\": " << finished_task_list[i]->getTask()->getId() << ","
        << std::endl;
        ofstream << "\t\t\t" << "\"data_in\": " << finished_task_list[i]->getTask()->getDataIn() << "," << std::endl;
        ofstream << "\t\t\t" << "\"data_out\": " << finished_task_list[i]->getTask()->getDataOut() << "," << std::endl;
        ofstream << "\t\t\t" << "\"can_offload\": " << finished_task_list[i]->getTask()->isOffload() << ","
                 << std::endl;
        ofstream << "\t\t\t" << "\"chosen_node_id\": " << finished_task_list[i]->getComputationNodeId() << ","
                 << std::endl;
        ofstream << "\t\t\t" << "\"source_node_id\": " << finished_task_list[i]->getTask()->getSourceMobileId() << ","
        << std::endl;
        ofstream << "\t\t\t" << "\"chosen_node_type\": " << "\""
                 << network.vertices.at(finished_task_list[i]->getComputationNodeId()).get()->printType() << "\"" << ","
                 << std::endl;
        ofstream << "\t\t\t" << "\"" << "input_upload_start_time" << "\": "
                 << finished_task_list[i]->getInputUploadStart() << "," << std::endl;
        ofstream << "\t\t\t" << "\"" << "input_upload_finish_time" << "\": "
                 << finished_task_list[i]->getInputUploadFinish() << "," << std::endl;
        ofstream << "\t\t\t" << "\"" << "process_start_time" << "\": " << finished_task_list[i]->getProcessingStart()
                 << "," << std::endl;
        ofstream << "\t\t\t" << "\"" << "process_finish_time" << "\": " << finished_task_list[i]->getProcessingFinish()
                 << "," << std::endl;
        ofstream << "\t\t\t" << "\"" << "output_upload_start_time" << "\": "
                 << finished_task_list[i]->getOutputUploadStart() << "," << std::endl;
        ofstream << "\t\t\t" << "\"" << "output_upload_finish_time" << "\": "
                 << finished_task_list[i]->getOutputUploadFinish() << "," << std::endl;

        ofstream << "\t\t\t" << "\"" << "parents" << "\": " << "[" << std::endl;

        for (int j = 0; j < finished_task_list[i]->parent_mappings.size(); j++) {
            ofstream << "\t\t\t\t" << "\"" << finished_task_list[i]->parent_mappings[j]->getTask()->getName() << "\"";

            if (j < finished_task_list[i]->parent_mappings.size() - 1)
                ofstream << "," << std::endl;
            else
                ofstream << std::endl;
        }

        ofstream << "\t\t\t" << "]," << std::endl;

        ofstream << "\t\t\t" << "\"" << "parent_upload_windows" << "\": " << "{" << std::endl;

        int map_size = 0;
        for (const auto&[node_id, window]: finished_task_list[i]->upload_times) {
            ofstream << "\t\t\t\t" << "\"" << node_id << "\": " << "[" << std::endl;

            for (int j = 0; j < window.size(); j++) {
                ofstream << "\t\t\t\t\t" << "[" << "\"" << window[j]->first << "\", " << "\"" << window[j]->second
                         << "\"" << "]";
                if (j < window.size() - 1)
                    ofstream << "," << std::endl;
                else
                    ofstream << std::endl;
            }

            ofstream << "\t\t\t\t" << "]";

            if (map_size < finished_task_list[i]->upload_times.size() - 1)
                ofstream << "," << std::endl;
            else
                ofstream << std::endl;
            map_size++;
        }

        ofstream << "\t\t\t" << "}" << std::endl;

        ofstream << "\t\t}";

        if (i < finished_task_list.size() - 1)
            ofstream << "," << std::endl;
        else
            ofstream << std::endl;
    }
}

void JsonOutputFunctions::edgeUploadWindows(Graph<ComputationNode, std::shared_ptr<EdgeData>> &network,
                                            std::ofstream &ofstream) {
    for(int i = 0; i < network.out_edges.size(); i++){
        for(int j = i + 1; j < network.out_edges.size(); j++){
            ofstream << "\t\t\"" << i << "_" << j << "\": " << "[" << std::endl;

            int upload_window_count = 0;
            for(const auto& upload_window: network.out_edges.at(i).at(j).get()->occupancy_times){
                ofstream << "\t\t\t[" << upload_window->first << ", " << upload_window->second << "]";

                if (upload_window_count < network.out_edges.at(i).at(j).get()->occupancy_times.size() - 1)
                    ofstream << "," << std::endl;
                else
                    ofstream << std::endl;
                upload_window_count++;
            }

            ofstream << "\t\t]";

            if (i == network.out_edges.size() - 2 && j == network.out_edges.size() - 1)
                ofstream << std::endl;
            else
                ofstream << "," << std::endl;
        }
    }
}
