//
// Created by Jamie Cotter on 22/01/2022.
//

#ifndef UNTITLED2_FILEREADER_H
#define UNTITLED2_FILEREADER_H

#include <string>
#include <vector>

namespace FileReader {
    std::vector<std::pair<float, std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>>>
    parseApplications(std::string file_name, float &time);

    std::vector<std::string> split_string(std::string source, char delimiter);
};


#endif //UNTITLED2_FILEREADER_H
