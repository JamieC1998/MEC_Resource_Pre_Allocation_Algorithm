//
// Created by Jamie Cotter on 21/01/2022.
//

#include "EdgeData.h"
#include <utility>

EdgeData::EdgeData(std::pair<int, int> edge, float latency, float bandwidth) : edge(std::move(edge)), latency(latency),
                                                                               bandwidth(bandwidth) {}

const std::pair<int, int> &EdgeData::getEdge() const {
    return edge;
}

float EdgeData::getLatency() const {
    return latency;
}

float EdgeData::getBandwidth() const {
    return bandwidth;
}

EdgeData::EdgeData(EdgeData *pData) {}

void EdgeData::clearTempOccupancyTimes() {
    this->temp_occupancy_times.clear();
}

void EdgeData::populateTempOccupancyTimes() {
    this->sortOccupancyTimes();

    std::copy(this->occupancy_times.begin(), this->occupancy_times.end(), std::back_inserter(temp_occupancy_times));
}

void EdgeData::sortOccupancyTimes() {
    std::sort(this->occupancy_times.begin(), this->occupancy_times.end(),
              [](const std::shared_ptr<std::pair<float, float>> &x,
                 const std::shared_ptr<std::pair<float, float>> &y) {
                  return x->second < y->first;
              });
}

