//
// Created by Jamie Cotter on 21/07/2021.
//

#include <sstream>
#include "MobileNode.h"

MobileNode::MobileNode(int cores, unsigned long long int millionsInstructionsPerCore, float ram, float storage,
                       node_type type, const std::pair<double, double> &coords)
        : EdgeNode(cores,
                   millionsInstructionsPerCore,
                   ram, storage,
                   type, coords) {}

int MobileNode::getBattery() const {
    return battery;
}

void MobileNode::setBattery(int battery) {
    MobileNode::battery = battery;
}

std::ostream &operator<<(std::ostream &os, const MobileNode &mN) {
    return (os << " Cores: " << mN.getCores() << "\n MIPS: " << mN.getMillionsInstructionsPerCore() << "\n Storage: "
               << mN.getStorage() << "\n RAM: " << mN.getRam() << "\n Type: " << mN.printType() << "\n Free: "
               << mN.isFree() << "\n Coords: ( "
               << mN.getCoords().first << ", " << mN.getCoords().second << ")" << "\n Battery: " << mN.getBattery()
               << "\n ID: " << mN.getId()
               << std::endl);
}

std::string MobileNode::to_string() const{
    std::stringstream ss;
    ss << (*this);
    return ss.str();
}

