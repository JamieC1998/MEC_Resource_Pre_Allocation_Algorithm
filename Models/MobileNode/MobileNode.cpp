//
// Created by Jamie Cotter on 21/07/2021.
//

#include "MobileNode.h"

int MobileNode::getBattery() const {
    return battery;
}

void MobileNode::setBattery(int battery) {
    MobileNode::battery = battery;
}

MobileNode::MobileNode(int cores, unsigned long long int millionsInstructionsPerCore, float ram, float storage,
                       node_type type, const std::pair<double, double> &coords)
        : EdgeNode(cores,
                   millionsInstructionsPerCore,
                   ram, storage,
                   type, coords) {}

