//
// Created by Jamie Cotter on 22/01/2022.
//

#ifndef UNTITLED2_TASK_H
#define UNTITLED2_TASK_H

#include <string>
#include "../../enums/TaskStates.h"
#include "../../enums/NodeTypes.h"

const float INSTRUCTION_SIZE_MEGABYTES = 0.000002;

class Task {
private:
    static int task_id_counter;
    static int application_id_counter;
    static int internal_id_counter;

    int id;
    int total_task_id;
    int application_id;

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

    task_state state = unprocessed;

    //True means it can be offloaded, false must be processed on source
    bool offload;

    float process_time_cloud;
    float process_time_edge;
    float process_time_mobile;

public:
    Task(std::string name, float process_time_cloud, float process_time_edge, float process_time_mobile,
               float ram, float dataIn, float dataOut, float storage, int source_mobile_id, bool can_offload);

    friend std::ostream &operator<<(std::ostream &os, const Task &task);

    std::string to_string();

    float getProcessTime(node_type nodeType);

    int getId() const;

    int getApplicationId() const;

    const std::string &getName() const;

    int getSourceMobileId() const;

    float getRam() const;

    float getDataIn() const;

    float getDataOut() const;

    float getStorage() const;

    task_state getState() const;

    void setState(task_state state);

    bool isOffload() const;

    static void increaseAppIDCounter();

    static void resetInternalIDCounter();

    int getTotalTaskId() const;

};


#endif //UNTITLED2_TASK_H
