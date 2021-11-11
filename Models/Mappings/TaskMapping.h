//
// Created by Jamie Cotter on 11/11/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_TASKMAPPING_H
#define FIRSTHOPMOBILEOFFLOADING_TASKMAPPING_H

#include <Models/VertexData/TaskVertexData.h>
#include <Models/VertexData/NetworkVertexData.h>

struct TaskMapping {
    float absoluteStart = 0;
    float absoluteFinish = 0;
    std::reference_wrapper<TaskVertexData> task;
    std::reference_wrapper<NetworkVertexData> node;
    float processStartTime = 0;
    float processFinishTime = 0;
};

#endif //FIRSTHOPMOBILEOFFLOADING_TASKMAPPING_H
