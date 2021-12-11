//
// Created by Jamie Cotter on 21/07/2021.
//

#include <sstream>
#include "Task.h"

int Task::task_id_counter = 0;

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

float Task::getStorage() const {
    return storage;
}

const std::string &Task::getName() const {
    return name;
}

void Task::setName(const std::string &name) {
    Task::name = name;
}

std::ostream &operator<<(std::ostream &os, const Task &task) {
    return (os << " Name: " << task.getName()
               << "\n RAM: " << task.getRam()
               << "\n CORES: " << task.getCoreCount()
               << "\n Data in: " << task.getDataIn()
               << "\n Data out: " << task.getDataOut()
               << "\n Storage: " << task.getStorage()
               << "\n Source Mobile ID: " << task.getSourceMobileId()
               << "\n Can Offload: " << task.isOffload()
               << "\n TASK ID: " << task.getId()
               << std::endl);
}

std::string Task::to_string() {
    std::stringstream ss;
    ss << (*this);
    return ss.str();
}

void Task::setStorage(float storage) {
    Task::storage = storage;
}

bool Task::isDone() const {
    return done;
}

void Task::setDone(bool done) {
    Task::done = done;
}

bool Task::isInProgress() const {
    return in_progress;
}

void Task::setInProgress(bool inProgress) {
    in_progress = inProgress;
}

int Task::getSourceMobileId() const {
    return source_mobile_id;
}

bool Task::isOffload() const {
    return offload;
}

int Task::getId() const {
    return id;
}

int Task::getCoreCount() const {
    return core_count;
}

float Task::getProcessTime(node_type nodeType) {
    if (nodeType == node_type::cloud)
        return this->process_time_cloud;
    else if (nodeType == node_type::edge)
        return this->process_time_edge;
    else if (nodeType == node_type::mobile)
        return this->process_time_mobile;
    return process_time_mobile;
}
