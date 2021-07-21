//
// Created by Jamie Cotter on 21/07/2021.
//

#include "EdgeNode.h"


EdgeNode::EdgeNode(int cores, unsigned long long int millionsInstructionsPerCore, float ram, float storage,
                   node_type type, const std::pair<double, double> &coords) : ComputationNode(cores,
                                                                                              millionsInstructionsPerCore,
                                                                                              ram, storage, type),
                                                                              coords(coords) {}

const std::pair<double, double> &EdgeNode::getCoords() const {
    return coords;
}

void EdgeNode::setCoords(const std::pair<double, double> &coords) {
    EdgeNode::coords = coords;

}

void EdgeNode::setLongitude(double longitude) {
    coords.first = longitude;
}

void EdgeNode::setLatitude(double latitude) {
    coords.second = latitude;
}

double EdgeNode::getLongitude() {
    return coords.first;
}

double EdgeNode::getLatitude() {
    return coords.second;
}
