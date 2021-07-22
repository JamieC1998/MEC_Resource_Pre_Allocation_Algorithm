//
// Created by Jamie Cotter on 21/07/2021.
//

#include <sstream>
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
}

unsigned int Task::getDataOut() const {
    return data_out;
}

void Task::setDataOut(unsigned int dataOut) {
    data_out = dataOut;
}

unsigned int Task::getStorage() const {
    return storage;
}

const std::string &Task::getName() const {
    return name;
}

void Task::setName(const std::string &name) {
    Task::name = name;
}

std::ostream &operator<<(std::ostream &os, const Task &task) {
    return (os << "Name: " << task.getName() << "\n RAM: " << task.getRam() << "\n MI: " << task.getMillionsOfInstructions() << "\n Data in: "
               << task.getDataIn() << "\n Data out: " << task.getRam() << "\n Storage: " << task.getStorage()
               << std::endl);
}

std::string Task::to_string() {
    std::stringstream ss;
    ss << (*this);
    return ss.str();
}

void Task::setStorage(unsigned int storage) {
    Task::storage = storage;
}

Task::Task(const std::string &name, unsigned long long int millionsOfInstructions, float ram, unsigned int dataIn,
           unsigned int dataOut, unsigned int storage) : name(name), millions_of_instructions(millionsOfInstructions),
                                                         ram(ram), data_in(dataIn), data_out(dataOut),
                                                         storage(storage) {}

