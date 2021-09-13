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

        bool runOnInput = false;
        bool runOnTimer = true;

        I_Task(pEnv* env);
        virtual int run()=0;
        virtual void nextRun(double time); // set next runtime

        bool lockEnv();
        void unlockEnv();
        bool isEnvLocked();
};

void I_Task::nextRun(double time) {
    this->runAfter = micros64() + time * 1000000;
}

I_Task::I_Task(pEnv* env) {
    this->env = env;
}

I_Task::~I_Task() {
    this->unlockEnv();
    delete this->env;
}

bool I_Task::lockEnv() {
    // std_err will not need to be reset, due to it being the last
    // interface to be locked
    int preLockIn = this->env->std_in->lockedBy;
    int preLockOut = this->env->std_out->lockedBy;
    if (
    !this->env->std_in->setLock(this->pid) ||
    !this->env->std_out->setLock(this->pid) ||
    !this->env->std_err->setLock(this->pid)) {
        this->env->std_in->lockedBy = preLockIn;
        this->env->std_out->lockedBy = preLockOut;
        return false;
    }
    return true;
}

void I_Task::unlockEnv() {
    int pid = this->pid;
    if (this->env->std_in->lockedBy == pid)
        this->env->std_in->lockedBy = 0;
    if (this->env->std_out->lockedBy == pid)
        this->env->std_out->lockedBy = 0;
    if (this->env->std_err->lockedBy == pid)
        this->env->std_err->lockedBy = 0;
}

bool I_Task::isEnvLocked() {
    int pid = this->pid;
    return 
        (this->env->std_in->lockedBy  && this->env->std_in->lockedBy  != pid) ||
        (this->env->std_out->lockedBy && this->env->std_out->lockedBy != pid) ||
        (this->env->std_err->lockedBy && this->env->std_err->lockedBy != pid);
}

#endif