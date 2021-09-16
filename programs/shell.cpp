#include "../core/programRegistry.hpp"
#include "../core/task.hpp"
#include "../drivers/deviceManager.hpp"
#include "shell.hpp"

bool __SHELL_INIT = false;
void program::shell::init() {
    // only init the shell program once
    if (__SHELL_INIT) return;
    __SHELL_INIT = true;

    os::programs.addProgram("shell", &program::shell::main);
    os::programs.addAlias("sh", "shell");
}

String program::shell::version = "\nShell 0.1.0";

int program::shell::main(pEnv env, std::vector<String>* args) {
    env.std_out->println(program::shell::version);

    pEnv* envPtr = new pEnv(env);

    program::shell::process* proc = new program::shell::process(envPtr);
    proc->name = "shell";
    globalEventloop.add(proc);
}

// takes a string command and executes programs based on parameters
int program::shell::execute(pEnv env, String command) {
    // TODO
    std::vector<String>* argList = os::programs.parseArgumentList(command);

    // just in case a empty string has been entered
    if (!argList->size()) return 0;
    
    String programName = argList->at(0);
    programRegistry::returnState returns = os::programs.runProgram(env, argList);
    if (!returns.found) env.std_err->println("Program '" + programName + "' not fund!");
    return returns.returncode;
}


// TASK CODE //

int program::shell::process::run() {
    this->nextRun(0.1);
    // delay(100);

    // make sure to only run, if the env is not occupied
    if (this->isEnvLocked()) return 0;

    if (REQ_LOW_PERFORMANCE) {
        this->nextRun(0.5);
    }

    if (!this->env->std_in->available()) return 0;

    String input = "";
    while (this->env->std_in->available()) {
        input += this->env->std_in->read();
    }
    int len = input.length()-1;

    Serial.println(String("#> ") + input[len]);
    
    if (input[len] == '\n') len--;
    input = input.substring(0,len);
    this->env->std_out->println(">>> "+input);
    if (input == ".lock") this->lockEnv();
    if (input == ".unlock") this->unlockEnv();
    program::shell::execute(*env,input);

    return 0;
}