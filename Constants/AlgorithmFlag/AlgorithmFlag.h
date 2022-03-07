//
// Created by Jamie Cotter on 04/03/2022.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_ALGORITHMFLAG_H
#define FIRSTHOPMOBILEOFFLOADING_ALGORITHMFLAG_H

#include <string>

#define FLAG_MOBILE_PRIORITY 0x1
#define FLAG_RESERVATION_CHECK 0x2
#define FLAG_PREALLOCATION_ALGORITHM 0x4
#define FLAG_PROACTIVE_ALGORITHM 0x8
#define FLAG_PARTITION_FUNCTION static_cast<unsigned int>(0x10 | FLAG_PROACTIVE_ALGORITHM)
#define FLAG_REACTIVE_BASIC 0x20
#define FLAG_REACTIVE_MOBILE 0x40 | FLAG_REACTIVE_BASIC

namespace AlgorithmFlag{
    extern unsigned int algorithm_mode;

    void setAlgorithmMode(std::string mode);
};

#endif //FIRSTHOPMOBILEOFFLOADING_ALGORITHMFLAG_H
