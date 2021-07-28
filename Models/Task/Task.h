//
// Created by Jamie Cotter on 21/07/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_TASK_H
#define FIRSTHOPMOBILEOFFLOADING_TASK_H


#include <iostream>

class Task {
private:
    std::string name;

    int source_mobile_id;

    unsigned long long int millions_of_instructions;
    float ram;

    unsigned int data_in;
    unsigned int data_out;
    float storage;

    bool done = false;
    bool in_progress = false;

    //True means it can be offloaded, false must be processed on source
    bool offload;

public:
    Task(const std::string &name, unsigned long long int millionsOfInstructions, float ram, unsigned int dataIn,
         unsigned int dataOut, unsigned int storage, int source_mobile_id, bool can_offload);

    bool isOffload() const;

    int getSourceMobileId() const;

    bool isInProgress() const;

    void setInProgress(bool inProgress);

    bool isDone() const;

    void setDone(bool done);

    const std::string &getName() const;

    void setName(const std::string &name);

    unsigned long long int getMillionsOfInstructions() const;

    void setMillionsOfInstructions(unsigned long long int millionsOfInstructions);

    float getRam() const;

    void setRam(float ram);

    unsigned int getDataIn() const;

    void setDataIn(unsigned int dataIn);

    unsigned int getDataOut() const;

    void setDataOut(unsigned int dataOut);

    float getStorage() const;

    void setStorage(float storage);

    friend std::ostream &operator<<(std::ostream &os, const Task &task);

    std::string to_string();
};


#endif //FIRSTHOPMOBILEOFFLOADING_TASK_H
