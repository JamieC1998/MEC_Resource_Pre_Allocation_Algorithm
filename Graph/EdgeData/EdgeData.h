//
// Created by Jamie Cotter on 21/01/2022.
//

#ifndef UNTITLED2_EDGEDATA_H
#define UNTITLED2_EDGEDATA_H


#include <utility>
#include <vector>

class EdgeData {
private:
    std::pair<int, int> edge;

    float latency{};
    float bandwidth{};

public:
    std::vector<std::shared_ptr<std::pair<float, float>>> occupancy_times;
    std::vector<std::shared_ptr<std::pair<float, float>>> temp_occupancy_times;

    EdgeData(std::pair<int, int> edge, float latency, float bandwidth);

    const std::pair<int, int> &getEdge() const;

    float getLatency() const;

    float getBandwidth() const;

    explicit EdgeData(EdgeData *pData);

    void clearTempOccupancyTimes();

    void populateTempOccupancyTimes();

    void sortOccupancyTimes();
};


#endif //UNTITLED2_EDGEDATA_H
