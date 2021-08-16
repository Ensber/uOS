#ifndef ITASK_HPP
#define ITASK_HPP

#include "../core/pEnv.hpp"

class I_Task
{
    public:
        virtual ~I_Task();

        pEnv* env;

        int state = 0;
        uint64_t runAfter = 0;
        int pid = 0;
        String name = "<unnamed>";

        I_Task(pEnv* env);
        virtual int run()=0;
        virtual void nextRun(double time); // set next runtime
};

void I_Task::nextRun(double time) {
    this->runAfter = micros64() + time * 1000000;
}

I_Task::I_Task(pEnv* env) {
    this->env = env;
}

I_Task::~I_Task() {
    delete this->env;
}

#endif