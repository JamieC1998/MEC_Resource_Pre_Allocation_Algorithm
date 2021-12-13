//
// Created by Jamie Cotter on 21/07/2021.
//

#include <sstream>
#include "EdgeNode.h"


EdgeNode::EdgeNode(int cores, float ram, float storage,
                   node_type type, const std::pair<double, double> &coords) : ComputationNode(cores,
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

std::ostream &operator<<(std::ostream &os, const EdgeNode &eN) {
    return (os << " Cores: " << eN.getCores() << "\n Storage: "
               << eN.getStorage() << "\n RAM: " << eN.getRam() << "\n Type: " << eN.printType() << "\n Free: "
               << eN.isFree() << "\n Coords: ( "
               << eN.getCoords().first << ", " << eN.getCoords().second << ")"  << "\n ID: " << eN.getId() << std::endl);
}

std::string EdgeNode::to_string() const{
    std::stringstream ss;
    ss << (*this);
    return ss.str();
}
