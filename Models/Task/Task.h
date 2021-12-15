//
// Created by Jamie Cotter on 21/07/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_TASK_H
#define FIRSTHOPMOBILEOFFLOADING_TASK_H


#include <iostream>
#include <enums/NodeTypes.h>

const float INSTRUCTION_SIZE_MEGABYTES = 0.000002;

class Task {
private:
    static int task_id_counter;
    int id;

    std::string name;

    int source_mobile_id;

    //Gigabytes
    float ram;

    //Megabytes
    float data_in;

    //Megabytes
    float data_out;

    //Megabytes
    float storage;

    bool done = false;
    bool in_progress = false;

    //True means it can be offloaded, false must be processed on source
    bool offload;

    float process_time_cloud;
    float process_time_edge;
    float process_time_mobile;

public:
    Task(const std::string &name, float process_time_cloud, float process_time_edge, float process_time_mobile, float ram, float dataIn,
         float dataOut, float storage, int source_mobile_id, bool can_offload);

    int getId() const;

    bool isOffload() const;

    int getSourceMobileId() const;

    bool isInProgress() const;

    void setInProgress(bool inProgress);

    bool isDone() const;

    void setDone(bool done);

    const std::string &getName() const;

    void setName(const std::string &name);

    float getRam() const;

    void setRam(float ram);

    float getDataIn() const;

    void setDataIn(float dataIn);

    float getDataOut() const;

    void setDataOut(float dataOut);

    float getStorage() const;

    void setStorage(float storage);

    friend std::ostream &operator<<(std::ostream &os, const Task &task);

    std::string to_string();

    float getProcessTime(node_type nodeType);
};


#endif //FIRSTHOPMOBILEOFFLOADING_TASK_H
