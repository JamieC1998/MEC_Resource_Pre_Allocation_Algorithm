//
// Created by Jamie Cotter on 04/08/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_FILEREADER_H
#define FIRSTHOPMOBILEOFFLOADING_FILEREADER_H

#include <string>
#include <vector>


class FileReader {

public:
    static std::vector<std::pair<float, std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>>>
    parseApplications(std::string file_name,
                      float *time);
};


#endif //FIRSTHOPMOBILEOFFLOADING_FILEREADER_H
