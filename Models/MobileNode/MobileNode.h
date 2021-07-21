//
// Created by Jamie Cotter on 21/07/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_MOBILENODE_H
#define FIRSTHOPMOBILEOFFLOADING_MOBILENODE_H


#include <Models/EdgeNode/EdgeNode.h>

class MobileNode : public EdgeNode {
public:
    int battery = 100;

    MobileNode(int cores, unsigned long long int millionsInstructionsPerCore, float ram, float storage, node_type type,
               const std::pair<double, double> &coords);

    int getBattery() const;

    void setBattery(int battery);
};


#endif //FIRSTHOPMOBILEOFFLOADING_MOBILENODE_H
