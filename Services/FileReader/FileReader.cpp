//
// Created by Jamie Cotter on 04/08/2021.
//

#include "FileReader.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>

std::vector<std::pair<float, std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>>>
FileReader::parseApplications(std::string file_name) {
    std::string myText;

    std::ifstream MyReadFile(file_name);

    getline(MyReadFile, myText);
    int applicationCount = std::stoi(myText);

    std::vector<std::pair<float, std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>>> applications;

    for (int i = 0; i < applicationCount; i++) {
        getline(MyReadFile, myText);
        int numberOfTasks = std::stoi(myText);

        getline(MyReadFile, myText);
        float offloadTime = std::stof(myText);

        std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> application;

        for (int j = 0; j < numberOfTasks; j++) {
            getline(MyReadFile, myText);
            std::vector<std::string> res_a;
            std::vector<std::string> res_b;

            boost::algorithm::split(res_a, myText, boost::is_any_of(" "));

            getline(MyReadFile, myText);

            if(!myText.empty())
                boost::algorithm::split(res_b, myText, boost::is_any_of(" "));

            application.emplace_back(res_a, res_b);
        }

        applications.emplace_back(offloadTime, application);
    }

    return applications;
}
