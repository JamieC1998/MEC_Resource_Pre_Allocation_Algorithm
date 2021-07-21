//
// Created by Jamie Cotter on 21/07/2021.
//

#include "Task.h"

unsigned long long int Task::getMillionsOfInstructions() const {
    return millions_of_instructions;
}

void Task::setMillionsOfInstructions(unsigned long long int millionsOfInstructions) {
    millions_of_instructions = millionsOfInstructions;
}

float Task::getRam() const {
    return ram;
}

void Task::setRam(float ram) {
    Task::ram = ram;
}

unsigned int Task::getDataIn() const {
    return data_in;
}

void Task::setDataIn(unsigned int dataIn) {
    data_in = dataIn;
    storage = data_in + data_out;
}

unsigned int Task::getDataOut() const {
    return data_out;
}

void Task::setDataOut(unsigned int dataOut) {
    data_out = dataOut;
    storage = data_in + data_out;
}

unsigned int Task::getStorage() const {
    return storage;
}

Task::Task(unsigned long long int millionsOfInstructions, float ram, unsigned int dataIn, unsigned int dataOut)
        : millions_of_instructions(millionsOfInstructions), ram(ram), data_in(dataIn),
          data_out(dataOut), storage(dataIn + dataOut) {}

