//
// Created by Jamie Cotter on 21/07/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_COMPUTATIONNODE_H
#define FIRSTHOPMOBILEOFFLOADING_COMPUTATIONNODE_H

#include <enums/NodeTypes.h>
#include <iostream>
#include <Models/Task/Task.h>
#include <vector>

class ComputationNode {
private:
    static int id_counter;
    int id;

    int cores;
    unsigned long long int millions_instructions_per_core;

    //Gigabytes
    float ram;

    //Megabytes
    float storage;

    std::vector<Task> taskVector;

    node_type type;
    bool is_free;

public:
    ComputationNode(int cores, unsigned long long int millionsInstructionsPerCore, float ram, float storage,
                    node_type type);
    int getId() const;

    const std::vector<Task> &getTaskVector() const;

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
