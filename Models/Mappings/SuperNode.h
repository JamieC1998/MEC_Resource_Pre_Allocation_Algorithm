//
// Created by Jamie Cotter on 03/12/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_SUPERNODE_H
#define FIRSTHOPMOBILEOFFLOADING_SUPERNODE_H

#include "TaskMapping.h"
#include "TempTask.h"
#include <enums/NodeTypes.h>

struct SuperNode {
    int cloud_edge_gpu_cap = 0;
    int cloud_edge_usage = 0;

    int mobile_gpu_cap = 0;
    int mobile_usage = 0;

    float total_ram = 0;
    float total_ram_usage = 0;

    std::vector<TempTask> task_list;

    void addCapacity(node_type type){
        if(type == node_type::cloud || type == node_type::edge)
                cloud_edge_gpu_cap++;

        else
            mobile_gpu_cap++;
    }

    void addTask(TempTask task){
        if(task.allocated_node == node_type::cloud || task.allocated_node == node_type::edge)
            cloud_edge_usage++;

        else
            mobile_usage++;

        int index = 0;
        for(; index < task_list.size() && task_list[index].finish_time > task.finish_time; index++);
        task_list.insert(begin(task_list) + index, task);

        total_ram_usage += task.task.getRam();
    }

    void removeTask(TempTask task){
        switch(task.allocated_node){
            case node_type::cloud:
                cloud_edge_usage--;
                break;
            case node_type::mobile:
                mobile_usage--;
                break;
        }

        int index = 0;
        for(; index < task_list.size() && task_list[index].task.getId() != task.task.getId(); index++);

        total_ram_usage -= task.task.getRam();
    }
};

#endif //FIRSTHOPMOBILEOFFLOADING_SUPERNODE_H
