//
// Created by Jamie Cotter on 21/07/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_EDGENODE_H
#define FIRSTHOPMOBILEOFFLOADING_EDGENODE_H


#include <Models/ComputationNode/ComputationNode.h>
#include <utility>

class EdgeNode : public ComputationNode {
private:
    //0th index is longitude, 1st is latitude
    std::pair<double, double> coords = std::make_pair(0, 0);

public:
    EdgeNode(int gpu_count, float ram, float storage, node_type type,
             const std::pair<double, double> &coords);

    friend std::ostream& operator << (std::ostream &os, const EdgeNode &eN);

    std::string to_string() const;

    const std::pair<double, double> &getCoords() const;

    void setCoords(const std::pair<double, double> &coords);

    void setLongitude(double longitude);

    void setLatitude(double latitude);

    double getLongitude();

    double getLatitude();
};


#endif //FIRSTHOPMOBILEOFFLOADING_EDGENODE_H
