//
// Created by Jamie Cotter on 04/03/2022.
//

#include "./AlgorithmFlag.h"

unsigned int AlgorithmFlag::algorithm_mode = FLAG_MOBILE_PRIORITY | FLAG_RESERVATION_CHECK | FLAG_PREALLOCATION_ALGORITHM;

void AlgorithmFlag::setAlgorithmMode(std::string algorithm_name) {
    AlgorithmFlag::algorithm_mode = 0;

    if(algorithm_name == "reactive_basic")
        AlgorithmFlag::algorithm_mode |= FLAG_REACTIVE_BASIC;
    else if(algorithm_name == "reactive_mobile")
        AlgorithmFlag::algorithm_mode |= FLAG_REACTIVE_MOBILE | FLAG_MOBILE_PRIORITY;
    else if(algorithm_name == "partition")
        AlgorithmFlag::algorithm_mode |= FLAG_MOBILE_PRIORITY | FLAG_RESERVATION_CHECK | FLAG_PARTITION_FUNCTION;
    else if(algorithm_name == "proactive")
        AlgorithmFlag::algorithm_mode |= FLAG_MOBILE_PRIORITY | FLAG_PROACTIVE_ALGORITHM | FLAG_RESERVATION_CHECK;
    else
        AlgorithmFlag::algorithm_mode |= FLAG_MOBILE_PRIORITY | FLAG_RESERVATION_CHECK | FLAG_PREALLOCATION_ALGORITHM;
}