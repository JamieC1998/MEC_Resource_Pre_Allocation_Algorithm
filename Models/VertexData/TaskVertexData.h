//
// Created by Jamie Cotter on 22/07/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_TASKVERTEXDATA_H
#define FIRSTHOPMOBILEOFFLOADING_TASKVERTEXDATA_H

#include <Models/Task/Task.h>
#include <boost/optional.hpp>

struct TaskVertexData {
    boost::optional<Task> task;
    bool done = false;
    int absoluteStart = 0;
    int absoluteFinish = 0;
};

#endif //FIRSTHOPMOBILEOFFLOADING_TASKVERTEXDATA_H
