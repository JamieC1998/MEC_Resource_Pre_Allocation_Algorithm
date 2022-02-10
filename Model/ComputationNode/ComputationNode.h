//
// Created by Jamie Cotter on 21/01/2022.
//

#ifndef UNTITLED2_COMPUTATIONNODE_H
#define UNTITLED2_COMPUTATIONNODE_H

#include <string>
#include <vector>
#include "../../enums/NodeTypes.h"
#include "../TaskMapping/TaskMapping.h"

class ComputationNode {
private:
    static int id_counter;
    int id;

    int gpu_count{};

    //Gigabytes
    float ram{};

    //Megabytes
    float storage{};

    node_type nodeType;

public:

    std::vector<std::shared_ptr<TaskMapping>> taskQueue;

    ComputationNode(int gpuCount, float ram, float storage, node_type nodeType);

    explicit ComputationNode(ComputationNode *pNode);

    int getId() const;

    int getGpuCount() const;

    float getRam() const;

    float getStorage() const;

    node_type getNodeType() const;

    std::string printType() const;

    friend std::ostream& operator << (std::ostream &os, const ComputationNode &cN);

    std::string to_string();

    std::vector<std::shared_ptr<TaskMapping>> getTaskQueue();

    void setTaskQueue(const std::vector<std::shared_ptr<TaskMapping>> &taskQueue);

};

#endif //UNTITLED2_COMPUTATIONNODE_H
