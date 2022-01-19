//
// Created by Jamie Cotter on 18/01/2022.
//

#include "ApplicationGraph.h"

ApplicationGraph::ApplicationGraph() {}

void ApplicationGraph::addTask(Task* task, int taskIndex, std::vector<int> parentList) {
    this->taskList.push_back(task);
    this->lookup_table.insert({task->getId(), taskIndex});

    std::vector<int> childList;
    if(this->childList.count(taskIndex) == 0)
        this->childList.insert({taskIndex, childList});

    this->parentList.insert({task->getId(), parentList});

    for(int i: parentList){
        if(this->childList.count(i) == 0){
            std::vector<int> tempList;
            tempList.push_back(taskIndex);
            this->childList.insert({i, tempList});
        }
        else
            this->childList.find(i)->second.push_back(taskIndex);
    }
}

Task* ApplicationGraph::fetchTaskById(int id) {
    int index = this->lookup_table.find(id)->second;
    return taskList[index];
}

Task* ApplicationGraph::fetchTaskByIndex(int index) {
    return taskList[index];
}

std::vector<int> ApplicationGraph::fetchTaskParentsByIndex(int index){
    return this->parentList.find(taskList[index]->getId())->second;
}

std::vector<int> ApplicationGraph::fetchTaskParentsById(int id){
    return this->parentList.find(id)->second;
}

int ApplicationGraph::taskCount() {
    return this->taskList.size();
}

std::vector<int> ApplicationGraph::fetchTaskChildrenByIndex(int index) {
    return this->childList.find(index)->second;
}

std::vector<int> ApplicationGraph::fetchTaskChildrenById(int id) {
    return this->childList.find(this->lookup_table.find(id)->second)->second;
};

std::vector<Task*> ApplicationGraph::fetchTasks(){
    return this->taskList;
}

int ApplicationGraph::fetchIndexById(int id) {
    return lookup_table.find(id)->second;
}

void ApplicationGraph::deleteTasks(){
    for(Task* t: this->taskList){
        delete t;
    }
}