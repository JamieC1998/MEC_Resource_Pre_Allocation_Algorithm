//
// Created by Jamie Cotter on 28/10/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_EDGEPROPERTYDATA_H
#define FIRSTHOPMOBILEOFFLOADING_EDGEPROPERTYDATA_H

#include <vector>

struct EdgePropertyData {
    float edge_weight;

    //Latency in seconds
    float latency;

    std::vector<std::pair<float, float>> occupancy_times;
};

#endif //FIRSTHOPMOBILEOFFLOADING_EDGEPROPERTYDATA_H
