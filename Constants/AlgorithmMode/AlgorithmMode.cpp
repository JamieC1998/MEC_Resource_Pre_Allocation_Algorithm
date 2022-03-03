//
// Created by Jamie Cotter on 04/02/2022.
//

#include "AlgorithmMode.h"
algorithm_type AlgorithmMode::MODE = algorithm_type::PREALLOCATION;

void AlgorithmMode::setAlgorithmMode(const std::string& algorithm_name) {
    if(algorithm_name == "reactive_basic")
        AlgorithmMode::MODE = algorithm_type::REACTIVE_BASIC;
    else if(algorithm_name == "reactive_mobile")
        AlgorithmMode::MODE = algorithm_type::REACTIVE_MOBILE;
    else if(algorithm_name == "partition")
        AlgorithmMode::MODE = algorithm_type::PARTITION;
    else if(algorithm_name == "proactive")
        AlgorithmMode::MODE = algorithm_type::PROACTIVE;
    else
        AlgorithmMode::MODE = algorithm_type::PREALLOCATION;
}
