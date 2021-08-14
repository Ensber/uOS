#include "task.hpp"

int dispatcher::add(I_Task* task) {
    taskData data;
    data.task = task;
    data.pid = this->pidCounter++;
    data.task->pid = data.pid; // tell the program, wich id it has got
    this->taskList.push_back(data);
}

bool dispatcher::kill(int pid) {
    for (int i=0; i<this->taskList.size(); i++) {
        if (this->taskList.at(i).pid == pid) {
            this->taskList.erase(this->taskList.begin() + i);
            return true;
        }
    }
    return false;
}

void dispatcher::clearTelemetry() {
    for (int i=0; i<this->taskList.size(); i++) {
        taskData cTask = this->taskList.at(i);
        cTask.runtime = 0;
    }
}

uint64_t dispatcher::run() {
    uint64_t dispatcher_start = micros64();
    uint64_t nextExecution = -1;
    for (int i=0; i<this->taskList.size(); i++) {
        taskData cTask = this->taskList.at(i);
        if (cTask.task->runAfter < nextExecution) 
            nextExecution = cTask.task->runAfter;
        //Serial.println(String()+"T"+cTask.pid+" "+(long)micros64()+", "+(long)cTask.task->runAfter);
        if (micros64() >= cTask.task->runAfter) {
            int returnCode;
            if (this->telemetry) {
                uint64_t start = micros64();
                returnCode = cTask.task->run();
                uint64_t runtime = micros64() - start;
                cTask.runtime += runtime;
                dispatcher_start += runtime;
                this->taskList[i] = cTask;
            } else {
                returnCode = cTask.task->run();
            }
            // kill task if the returncode is not 0
            if (returnCode) {
                Serial.println(cTask.task->name + " stopped");
                this->taskList.erase(this->taskList.begin() + i);
            }
        }
    }
    this->dispatcher_runtime += micros64() - dispatcher_start;
    return nextExecution;
}

strippedTaskDataContainer dispatcher::getTelemetry() {
    strippedTaskDataContainer container;
    container.size = this->taskList.size() + 1;
    container.data = new strippedTaskData[container.size];

    container.data[0].pid = 0;
    container.data[0].runtime = this->dispatcher_runtime;
    container.data[0].state = 0;
    container.data[0].name = String()+"[dispatcher]";

    for (int i=1; i<container.size; i++) {
        taskData cTask = this->taskList.at(i-1);
        container.data[i].pid = cTask.pid;
        container.data[i].runtime = cTask.runtime;
        container.data[i].state = cTask.task->state;
        container.data[i].name = cTask.task->name;
    }

    return container;
}