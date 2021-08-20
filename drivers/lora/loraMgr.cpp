#include "../../core/programRegistry.hpp"
#include "../../core/task.hpp"
#include "loraMgr.hpp"

bool __loraMgr_INIT = false;
void program::loraMgr::init() {
    // only init the loraMgr program once
    if (__loraMgr_INIT) return;
    __loraMgr_INIT = true;

    os::programs.addProgram("lora", &program::loraMgr::main);
}

String program::loraMgr::version = "\nloraMgr 0.1.0";

int program::loraMgr::main(pEnv env, std::vector<String>* args) {
    if (env.std_out->lockedBy == 0)
        env.std_out->println(program::loraMgr::version);

    // create a task
    pEnv* envPtr = new pEnv(env);
    program::loraMgr::process* proc = new program::loraMgr::process(envPtr);
    proc->name = "lora";
    globalEventloop.add(proc);
}

// TASK CODE //

int program::loraMgr::process::run() {
    this->nextRun(0.1);
    if (this->isEnvLocked()) return 0;

    
}