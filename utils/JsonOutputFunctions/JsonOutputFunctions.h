//
// Created by Jamie Cotter on 07/02/2022.
//

#ifndef UNTITLED2_JSONOUTPUTFUNCTIONS_H
#define UNTITLED2_JSONOUTPUTFUNCTIONS_H


#include <vector>
#include "../../Model/TaskMapping/TaskMapping.h"
#include "../../Graph/Graph.h"
#include "../../Model/ComputationNode/ComputationNode.h"
#include "../../Graph/EdgeData/EdgeData.h"

namespace JsonOutputFunctions {
    void outputJson(std::vector<std::shared_ptr<TaskMapping>> &finished, std::vector<std::shared_ptr<Graph<Task, bool>>>& processing_applications, std::vector<std::pair<float, std::shared_ptr<Graph<Task, bool>>>>& unprocessed_applications, const char* output_file_name, Graph<ComputationNode, std::shared_ptr<EdgeData>>& nG);

    int totalTaskCount(std::vector<std::shared_ptr<Graph<Task, bool>>> &processing_apps,
                              std::vector<std::pair<float, std::shared_ptr<Graph<Task, bool>>>> &unprocessed_apps);

    void outputTaskData(std::vector<std::shared_ptr<TaskMapping>> &finished_task_list, std::ofstream &ofstream,
                               Graph<struct ComputationNode, std::shared_ptr<EdgeData>>& network);

    void edgeUploadWindows(Graph<ComputationNode, std::shared_ptr<EdgeData>> &network, std::ofstream &ofstream);
};


#endif //UNTITLED2_JSONOUTPUTFUNCTIONS_H
