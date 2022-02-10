//
// Created by Jamie Cotter on 04/08/2021.
//

#include "FileReader.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

std::vector<std::pair<float, std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>>>
FileReader::parseApplications(std::string file_name, float& time) {
    std::string myText;

    std::ifstream MyReadFile(file_name);

    getline(MyReadFile, myText);
    time = std::stof(myText) * 1000;

    getline(MyReadFile, myText);
    int applicationCount = std::stoi(myText);

    std::vector<std::pair<float, std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>>> applications;

    for (int i = 0; i < applicationCount; i++) {
        getline(MyReadFile, myText);
        int numberOfTasks = std::stoi(myText);

        getline(MyReadFile, myText);
        float offloadTime = std::stof(myText) * 1000;

        std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> application;

        for (int j = 0; j < numberOfTasks; j++) {
            getline(MyReadFile, myText);
            std::vector<std::string> res_a;
            std::vector<std::string> res_b;

            res_a = FileReader::split_string(myText, ' ');

            getline(MyReadFile, myText);

            if(!myText.empty())
                res_b = FileReader::split_string(myText, ' ');

            application.emplace_back(res_a, res_b);
        }

        applications.emplace_back(offloadTime, application);
    }

    return applications;
}

std::vector<std::string> FileReader::split_string(std::string source, char delimiter) {
    std::vector<std::string> words;

    std::stringstream ss(source);
    std::string tmp;

    while(std::getline(ss, tmp, delimiter))
        words.push_back(tmp);

    return words;
}


