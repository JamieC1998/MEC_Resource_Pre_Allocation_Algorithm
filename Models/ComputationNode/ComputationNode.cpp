//
// Created by Jamie Cotter on 21/07/2021.
//

#include "ComputationNode.h"
#include <iostream>
#include <sstream>

int ComputationNode::id_counter = 0;

ComputationNode::ComputationNode(int gpu_count, float ram, float storage, node_type type)
        : gpu_count(gpu_count), ram(ram), storage(storage),
          type(type), is_free(true), id(id_counter) {
    id_counter++;
}

int ComputationNode::getGPU_Count() const {
    return gpu_count;
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
    return (os << " Cores: " << cN.getGPU_Count() << << "\n Storage: "
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

const std::vector<NodeMapping> &ComputationNode::getReservations() const {
    return reservations;
}

void ComputationNode::setReservations(const std::vector<NodeMapping> &reservations) {
    ComputationNode::reservations = reservations;
}



