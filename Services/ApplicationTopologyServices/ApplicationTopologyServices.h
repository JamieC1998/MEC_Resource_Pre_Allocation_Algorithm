//
// Created by Jamie Cotter on 22/01/2022.
//

#ifndef UNTITLED2_APPLICATIONTOPOLOGYSERVICES_H
#define UNTITLED2_APPLICATIONTOPOLOGYSERVICES_H


#include <vector>
#include "../../Graph/Graph.h"
#include "../../Model/Task/Task.h"

class ApplicationTopologyServices {
public:
    static std::vector<std::pair<float, std::shared_ptr<Graph<Task, bool>>>>
    generateApplications(const char *input_file, float &simulation_time, int source_mobile_id);

    static std::shared_ptr<Graph<Task, bool>> generateApplication(
            std::pair<float, std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>> item,
            int source_mobile_id);
};


#endif //UNTITLED2_APPLICATIONTOPOLOGYSERVICES_H
