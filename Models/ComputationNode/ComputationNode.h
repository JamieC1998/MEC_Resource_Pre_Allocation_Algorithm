//
// Created by Jamie Cotter on 21/07/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_COMPUTATIONNODE_H
#define FIRSTHOPMOBILEOFFLOADING_COMPUTATIONNODE_H

#include <enums/NodeTypes.h>
#include <iostream>

class ComputationNode {
private:
    int cores;
    unsigned long long int millions_instructions_per_core;
    float ram;
    float storage;
    node_type type;
    bool is_free;

public:
    ComputationNode(int cores, unsigned long long int millionsInstructionsPerCore, float ram, float storage,
                    node_type type);

    friend std::ostream& operator << (std::ostream &os, const ComputationNode &cN);

    std::string to_string();

    int getCores() const;

    unsigned long long int getMillionsInstructionsPerCore() const;

    float getRam() const;

    float getStorage() const;

    node_type getType() const;

    bool isFree() const;

    void setIsFree(bool isFree);

    std::string printType() const;
};


#endif //FIRSTHOPMOBILEOFFLOADING_COMPUTATIONNODE_H
