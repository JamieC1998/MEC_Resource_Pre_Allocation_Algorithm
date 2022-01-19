//
// Created by Jamie Cotter on 18/01/2022.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_APPLICATIONGRAPH_H
#define FIRSTHOPMOBILEOFFLOADING_APPLICATIONGRAPH_H


#include <vector>
#include <unordered_map>
#include <Models/Task/Task.h>

class ApplicationGraph {
    private:
        std::unordered_map<int, int> lookup_table;

        std::unordered_map<int, std::vector<int>> childList;
        std::unordered_map<int, std::vector<int>> parentList;

    public:
        ApplicationGraph();

        std::vector<Task*> taskList;

        Task* fetchTaskByIndex(int index);
        Task* fetchTaskById(int id);

        void addTask(Task *task, int taskIndex, std::vector<int> parentList);

        std::vector<int> fetchTaskParentsByIndex(int index);

        std::vector<int> fetchTaskParentsById(int id);

        std::vector<int> fetchTaskChildrenByIndex(int index);

        std::vector<int> fetchTaskChildrenById(int id);

        int taskCount();

        std::vector<Task*> fetchTasks();

        int fetchIndexById(int id);

        void deleteTasks();

};


#endif //FIRSTHOPMOBILEOFFLOADING_APPLICATIONGRAPH_H
