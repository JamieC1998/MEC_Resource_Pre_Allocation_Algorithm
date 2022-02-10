//
// Created by Jamie Cotter on 21/01/2022.
//

#include <iostream>
#include <sstream>
#include "ComputationNode.h"

int ComputationNode::id_counter = 0;

ComputationNode::ComputationNode(int gpuCount, float ram, float storage, node_type nodeType) : gpu_count(gpuCount),
                                                                                               ram(ram),
                                                                                               storage(storage),
                                                                                               nodeType(nodeType),
                                                                                               id(id_counter) {
    id_counter++;
}

ComputationNode::ComputationNode(ComputationNode *pNode) {}

int ComputationNode::getId() const {
    return id;
}

int ComputationNode::getGpuCount() const {
    return gpu_count;
}

float ComputationNode::getRam() const {
    return ram;
}

float ComputationNode::getStorage() const {
    return storage;
}

node_type ComputationNode::getNodeType() const {
    return nodeType;
}

std::ostream &operator<<(std::ostream &os, const ComputationNode &cN) {
    return (os << " GPUS: " << cN.getGpuCount() << "\n Storage: "
               << cN.getStorage() << "\n RAM: " << cN.getRam() << "\n Type: " << cN.printType() << "\n ID: "
               << cN.getId() << std::endl);
}

std::string ComputationNode::to_string() {
    std::stringstream ss;
    ss << (*this);
    return ss.str();
}

std::string ComputationNode::printType() const {
    if (this->nodeType == cloud)
        return "cloud";
    else if (this->nodeType == edge)
        return "edge";
    else if (this->nodeType == mobile)
        return "mobile";
    return "empty";
}


void ComputationNode::setTaskQueue(const std::vector<std::shared_ptr<TaskMapping>> &taskQueue) {
    ComputationNode::taskQueue = taskQueue;
}

std::vector<std::shared_ptr<TaskMapping>> ComputationNode::getTaskQueue() {
    return this->taskQueue;
}

