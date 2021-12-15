//
// Created by Jamie Cotter on 17/08/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_RESERVATIONMAPPING_H
#define FIRSTHOPMOBILEOFFLOADING_RESERVATIONMAPPING_H

#include <Models/VertexData/TaskVertexData.h>
#include <vector>

struct TaskMapping;

struct ReservationMapping {
    TaskVertexData* task;
    int nodeIndex;
    float finishTime;
    float startTime;
    std::vector<TaskMapping> parents;
    int parentCount;
    float uploadStart = 0;
    float uploadFinish = 0;
};

#endif //FIRSTHOPMOBILEOFFLOADING_RESERVATIONMAPPING_H
