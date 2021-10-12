//
// Created by Jamie Cotter on 21/07/2021.
//

#include "ComputationNode.h"
#include <iostream>
#include <sstream>

int ComputationNode::id_counter = 0;

ComputationNode::ComputationNode(int cores, unsigned long long int millionsInstructionsPerCore, float ram,
                                 float storage, node_type type)
        : cores(cores), millions_instructions_per_core(millionsInstructionsPerCore), ram(ram), storage(storage),
          type(type), is_free(true), id(id_counter) {
    id_counter++;
}

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

void ComputationNode::setIsFree(bool isFree) {
    is_free = isFree;
}

bool ComputationNode::isFree() const {
    return is_free;
}

std::ostream &operator<<(std::ostream &os, const ComputationNode &cN) {
    return (os << " Cores: " << cN.getCores() << "\n MIPS: " << cN.getMillionsInstructionsPerCore() << "\n Storage: "
               << cN.getStorage() << "\n RAM: " << cN.getRam() << "\n Type: " << cN.printType() << "\n Free: "
               << cN.isFree() << "\n ID: " << cN.getId() << std::endl);
}

std::string ComputationNode::to_string() {
    std::stringstream ss;
    ss << (*this);
    return ss.str();
}

std::string ComputationNode::printType() const {
    if (type == cloud)
        return "cloud";
    else if (type == edge)
        return "edge";
    else if (type == mobile)
        return "mobile";
    return "empty";
}

int ComputationNode::getId() const {
    return id;
}

const std::vector<NodeMapping> &ComputationNode::getTaskVector() const {
    return taskVector;
}



