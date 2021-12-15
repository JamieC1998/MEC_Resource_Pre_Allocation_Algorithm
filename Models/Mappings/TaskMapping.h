//
// Created by Jamie Cotter on 17/08/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_TASKMAPPING_H
#define FIRSTHOPMOBILEOFFLOADING_TASKMAPPING_H

#include <Models/VertexData/TaskVertexData.h>
#include <Models/VertexData/NetworkVertexData.h>

struct NetworkVertexData;

struct TaskMapping {
    float absoluteStart = 0;
    float absoluteFinish = 0;
    std::reference_wrapper<TaskVertexData> task;
    NetworkVertexData* node;
    int nodeIndex = -1;
    float uploadStart = 0;
    float uploadFinish = 0;
};

#endif //FIRSTHOPMOBILEOFFLOADING_TASKMAPPING_H
