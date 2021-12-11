//
// Created by Jamie Cotter on 21/07/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_MOBILENODE_H
#define FIRSTHOPMOBILEOFFLOADING_MOBILENODE_H

#include <Models/EdgeNode/EdgeNode.h>

class MobileNode : public EdgeNode {
private:
    int battery = 100;

public:
    MobileNode(int gpu_count, float ram, float storage, node_type type,
               const std::pair<double, double> &coords);

    friend std::ostream& operator << (std::ostream &os, const MobileNode &mN);

    std::string to_string() const;

    int getBattery() const;

    void setBattery(int battery);
};


#endif //FIRSTHOPMOBILEOFFLOADING_MOBILENODE_H
