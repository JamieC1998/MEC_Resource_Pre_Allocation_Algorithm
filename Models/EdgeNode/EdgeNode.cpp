//
// Created by Jamie Cotter on 21/07/2021.
//

#include "EdgeNode.h"

double EdgeNode::getCoordX() const {
    return coord_longitude;
}

void EdgeNode::setCoordX(double coordX) {
    coord_longitude = coordX;
}

double EdgeNode::getCoordY() const {
    return coord_latitude;
}

void EdgeNode::setCoordY(double coordY) {
    coord_latitude = coordY;
}

void EdgeNode::setCoords(double x, double y) {
    coord_longitude = x;
    coord_latitude = y;
}

EdgeNode::EdgeNode(int cores, unsigned long long int millionsInstructionsPerCore, float ram, float storage,
                   node_type type, double coordLongitude, double coordLatitude)
        : ComputationNode(cores, millionsInstructionsPerCore, ram, storage, type),
          coord_longitude(coordLongitude), coord_latitude(coordLatitude) {}
