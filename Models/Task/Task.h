//
// Created by Jamie Cotter on 21/07/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_TASK_H
#define FIRSTHOPMOBILEOFFLOADING_TASK_H


class Task {
private:
    unsigned long long int millions_of_instructions;
    float ram;
    unsigned int data_in;
    unsigned int data_out;
    unsigned int storage;

public:

    Task(unsigned long long int millionsOfInstructions, float ram, unsigned int dataIn, unsigned int dataOut);

    unsigned long long int getMillionsOfInstructions() const;

    void setMillionsOfInstructions(unsigned long long int millionsOfInstructions);

    float getRam() const;

    void setRam(float ram);

    unsigned int getDataIn() const;

    void setDataIn(unsigned int dataIn);

    unsigned int getDataOut() const;

    void setDataOut(unsigned int dataOut);

    unsigned int getStorage() const;
};


#endif //FIRSTHOPMOBILEOFFLOADING_TASK_H
