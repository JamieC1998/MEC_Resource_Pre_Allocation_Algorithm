//
// Created by Jamie Cotter on 21/07/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_EDGENODE_H
#define FIRSTHOPMOBILEOFFLOADING_EDGENODE_H


#include <Models/ComputationNode/ComputationNode.h>

class EdgeNode : public ComputationNode {
public:
    double coord_longitude;
    double coord_latitude;

    EdgeNode(int cores, unsigned long long int millionsInstructionsPerCore, float ram, float storage, node_type type,
             double coordLongitude, double coordLatitude);

    double getCoordX() const;

    void setCoordX(double coordX);

    double getCoordY() const;

    void setCoordY(double coordY);

    void setCoords(double x, double y);
};


#endif //FIRSTHOPMOBILEOFFLOADING_EDGENODE_H
