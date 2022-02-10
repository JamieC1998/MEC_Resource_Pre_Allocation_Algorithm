//
// Created by Jamie Cotter on 04/02/2022.
//

#include "AlgorithmMode.h"
algorithm_type AlgorithmMode::MODE = PREALLOCATION;

void AlgorithmMode::setAlgorithmMode(std::string algorithm_name) {
    if(algorithm_name == "reactive_basic")
        AlgorithmMode::MODE = REACTIVE_BASIC;
    else if(algorithm_name == "reactive_mobile")
        AlgorithmMode::MODE = REACTIVE_MOBILE;
    else if(algorithm_name == "partition")
        AlgorithmMode::MODE = PARTITION;
    else if(algorithm_name == "proactive")
        AlgorithmMode::MODE = PROACTIVE;
    else
        AlgorithmMode::MODE = PREALLOCATION;
}
