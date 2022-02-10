//
// Created by Jamie Cotter on 22/01/2022.
//

#include "ApplicationTopologyServices.h"
#include "../../utils/FileReader/FileReader.h"

std::vector<std::pair<float, std::shared_ptr<Graph<Task, bool>>>>
ApplicationTopologyServices::generateApplications(const char *input_file, float &simulation_time, int source_mobile_id) {
    auto parsed_applications = FileReader::parseApplications(input_file, simulation_time);
    std::vector<std::pair<float, std::shared_ptr<Graph<Task, bool>>>> applications;

    applications.reserve(parsed_applications.size());

    for (const auto &application: parsed_applications)
        applications.emplace_back(application.first, generateApplication(application, source_mobile_id));

    return applications;
}

std::shared_ptr<Graph<Task, bool>> ApplicationTopologyServices::generateApplication(
        std::pair<float, std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>> item,
        int source_mobile_id) {

    std::shared_ptr<Graph<Task, bool>> application_ptr = std::make_shared<Graph<Task, bool>>(Graph<Task, bool>(true));
    Task::resetInternalIDCounter();

    for (int j = 0; j < item.second.size(); j++) {
        auto &application = item.second[j];
        std::vector<std::string> strL = application.first;
        std::vector<std::string> edgeL = application.second;
        Task *generated_task = new Task(strL[0], stof(strL[1]) * 1000, stof(strL[2]) * 1000, stof(strL[3]) * 1000,
                                        stof(strL[4]), stof(strL[5]), stof(strL[6]),
                                        stof(strL[7]), source_mobile_id, stoi(strL[8]));

        application_ptr->addVertex(generated_task);

        for (auto &i : edgeL)
            application_ptr->addEdge(stoi(i), j, true);
    }

    Task::increaseAppIDCounter();
    return application_ptr;
}
