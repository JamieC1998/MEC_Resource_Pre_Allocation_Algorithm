//
// Created by Jamie Cotter on 04/02/2022.
//

#ifndef UNTITLED2_ALGORITHMMODE_H
#define UNTITLED2_ALGORITHMMODE_H


#include "../../enums/AlgorithmTypes.h"
#include <string>

class AlgorithmMode {
public:
    static algorithm_type MODE;

    static void setAlgorithmMode(const std::string& algorithm_name);
};


#endif //UNTITLED2_ALGORITHMMODE_H
