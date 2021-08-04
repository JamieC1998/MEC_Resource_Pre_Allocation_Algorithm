//
// Created by Jamie Cotter on 04/08/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_FILEREADER_H
#define FIRSTHOPMOBILEOFFLOADING_FILEREADER_H


#include <string>

class FileReader {

public:
    static std::vector<std::pair<float, std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>>>
    parseApplications(std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> file_name);
};


#endif //FIRSTHOPMOBILEOFFLOADING_FILEREADER_H
