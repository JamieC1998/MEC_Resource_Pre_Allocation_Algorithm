//
// Created by Jamie Cotter on 21/07/2021.
//

#include "ComputationNode.h"

int ComputationNode::getCores() const {
    return cores;
}

unsigned long long int ComputationNode::getMillionsInstructionsPerCore() const {
    return millions_instructions_per_core;
}

float ComputationNode::getRam() const {
    return ram;
}

float ComputationNode::getStorage() const {
    return storage;
}

node_type ComputationNode::getType() const {
    return type;
}

bool ComputationNode::isFree() const {
    return is_free;
}

void ComputationNode::setIsFree(bool isFree) {
    is_free = isFree;
}

ComputationNode::ComputationNode(int cores, unsigned long long int millionsInstructionsPerCore, float ram,
                                 float storage, node_type type)
        : cores(cores), millions_instructions_per_core(millionsInstructionsPerCore), ram(ram), storage(storage),
          type(type) {};
