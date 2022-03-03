//
// Created by Jamie Cotter on 22/01/2022.
//

#include <iostream>
#include <sstream>
#include <utility>
#include "Task.h"

int Task::application_id_counter = 0;
int Task::task_id_counter = 0;
int Task::internal_id_counter = 0;

Task::Task(std::string name, float process_time_cloud, float process_time_edge, float process_time_mobile,
           float ram, float dataIn, float dataOut, float storage, int source_mobile_id, bool can_offload): name(std::move(name)), process_time_cloud(process_time_cloud), process_time_edge(process_time_edge),
           process_time_mobile(process_time_mobile), ram(ram), data_in(dataIn), data_out(dataOut), storage(storage), source_mobile_id(source_mobile_id), offload(can_offload), id(internal_id_counter), application_id(application_id_counter), total_task_id(task_id_counter){
    task_id_counter++;
    internal_id_counter++;
}

float Task::getProcessTime(node_type nodeType) const{
    if (nodeType == node_type::cloud)
        return this->process_time_cloud;
    else if (nodeType == node_type::edge)
        return this->process_time_edge;
    else if (nodeType == node_type::mobile)
        return this->process_time_mobile;
    return process_time_mobile;
}

std::ostream &operator<<(std::ostream &os, const Task &task) {
    return (os << " Name: " << task.getName()
    << "\n RAM: " << task.getRam()
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

int Task::getId() const {
    return id;
}

int Task::getApplicationId() const {
    return application_id;
}

const std::string &Task::getName() const {
    return name;
}

int Task::getSourceMobileId() const {
    return source_mobile_id;
}

float Task::getRam() const {
    return ram;
}

float Task::getDataIn() const {
    return data_in;
}

float Task::getDataOut() const {
    return data_out;
}

float Task::getStorage() const {
    return storage;
}

task_state Task::getState() const {
    return state;
}

bool Task::isOffload() const {
    return offload;
};

void Task::increaseAppIDCounter() {
    application_id_counter++;
}

void Task::setState(task_state state) {
    Task::state = state;
}

void Task::resetInternalIDCounter() {
    internal_id_counter = 0;
}

int Task::getTotalTaskId() const {
    return total_task_id;
}
