#ifndef TASK_HPP
#define TASK_HPP

#include "../interfaces/Task.hpp"

#include <vector>

struct taskData
{
    I_Task* task = nullptr;
    uint64_t runtime = 0;
    unsigned int longestCycleTime = 0;
    int pid;
};

struct strippedTaskData {
    uint64_t runtime;
    int state;
    int pid;
    String name;
    unsigned int longestCycleTime = 0;
};

struct strippedTaskDataContainer
{
    strippedTaskData* data; // array
    int size;
};

class dispatcher {
    private:
        std::vector<taskData> taskList;
        int pidCounter = 1;
        uint64_t dispatcher_runtime = 0;
        std::vector<int> killList;
    public:
        dispatcher();
        int add(I_Task* task);
        bool kill(int pid);
        uint64_t run();
        strippedTaskDataContainer getTelemetry();

        bool telemetry = true;
        void clearTelemetry();
};

dispatcher globalEventloop;

#endif