//
// Created by Jamie Cotter on 03/12/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_TEMPTASK_H
#define FIRSTHOPMOBILEOFFLOADING_TEMPTASK_H

#include <enums/NodeTypes.h>
#include "TaskMapping.h"

struct TempTask {
    std::vector<int> parents;
    std::vector<int> children;
    Task task;
    float finish_time = 0.0f;
    bool offloaded;
    bool finished;
    node_type allocated_node;
};

#endif //FIRSTHOPMOBILEOFFLOADING_TEMPTASK_H
