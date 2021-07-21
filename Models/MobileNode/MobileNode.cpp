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
                       node_type type, double coordLongitude, double coordLatitude)
        : EdgeNode(cores,
                   millionsInstructionsPerCore,
                   ram,
                   storage,
                   type,
                   coordLongitude,
                   coordLatitude) {}
