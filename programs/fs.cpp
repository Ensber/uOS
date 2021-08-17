#include "../core/programRegistry.hpp"
#include "../core/task.hpp"
#include "fs.hpp"

bool __fs_INIT = false;
void program::fs::init() {
    // only init the fs program once
    if (__fs_INIT) return;
    __fs_INIT = true;

    os::programs.addProgram("fs", &program::fs::main);
}

String program::fs::version = "\nfs 0.1.0";

int program::fs::main(pEnv env, std::vector<String>* args) {
    if (env.std_out->lockedBy == 0)
        env.std_out->println(program::fs::version);

    if (args->size() > 0 && args->at(0) == "i") {
        // create a task
        pEnv* envPtr = new pEnv(env);
        program::fs::process* proc = new program::fs::process(envPtr);
        proc->name = "fs";
        if (!proc->lockEnv()) {
            // abort, enviroment is not free
            delete proc;
            return 1;
        }
        globalEventloop.add(proc);
    }
}

program::fs::process::~process() {
    uFS.close(this->cFile);
    Serial.println("destroyed fs process correctly.");
}

// TASK CODE //

int program::fs::process::run() {
    this->nextRun(0.05);
    unsigned long procStartMillis = millis();

    if (!this->lockEnv()) {
        // abort, enviroment is not free
        return 1;
    }
    
    String input = "";
    if (state == 0 || state == 2) {
        while (this->env->std_in->available()) {
            input += this->env->std_in->read();
        }
    }
    
    String cStateInfo;

    int c; // -> unwritten character
    switch (this->state)
    {
    case 0: // read line
        if (input.length() > 0) {
            this->line += input;
            break;
        } else if (this->line.length() > 0) {
            if (this->argList != nullptr)
                delete this->argList;
            // remove newline at the end of the input
            if (this->line[this->line.length()-1] == '\n')
                this->line = this->line.substring(0, this->line.length()-1);
            if (this->line[this->line.length()-1] == '\r')
                this->line = this->line.substring(0,this->line.length()-1);
    
            this->argList = os::programs.parseArgumentList(this->line);
            this->env->std_out->println("fs> command: '"+this->line+"'");
            this->line = "";
            this->state = 1; // parse line
            this->nextRun(0); // go immediantly to the next stepp
        }
        break;
    case 1: // parse line
    {
        this->state = 0; // default to read line

        cStateInfo = " (state 1/get parameter)";
        if (this->argList==nullptr) {
            Serial.println("Somehow, the nullpointer retured...");
            Serial.flush();
            break;
        }
        if (this->argList->size() < 1) { this->env->std_out->println("fs> required 1 argument" + cStateInfo); break; }
        if (this->argList->at(0) == ".close") {
            uFS.close(this->cFile);
            this->env->std_out->println("fs> ok" + cStateInfo);
            this->fileOpen = false;
            break;
        }
        if (this->argList->at(0) == ".read") {
            if (!this->fileOpen) {
                this->env->std_out->println("fs> no file open" + cStateInfo);
                break;
            }
            this->state = 3; // write file
            this->env->std_out->println(String()+"fs> reading " + this->cFile.size() + " Bytes" + cStateInfo);
            break;
        }
        if (this->argList->at(0) == ".fsinfo") {
            if (uFS.sdCardAttached) {
                FSInfo64 info;
                LittleFS.info64(info);
                this->env->std_out->println(String()+"fs> {\"name\":\"Internal Storage\",\"attached\":true,\"total\":"+
                                            (unsigned long)info.totalBytes+",\"used\":"+(unsigned long)info.usedBytes+"}");
            } else {
                this->env->std_out->println(String()+"fs> {\"name\":\"Internal Storage\",\"attached\":false}");
            }
            if (uFS.sdCardAttached) {
                this->env->std_out->println(String()+"fs> {\"name\":\"SD-Card\",\"attached\":true,\"total\":"+
                                            (unsigned long)SD.size64()+",\"used\":-1}");
            } else {
                this->env->std_out->println(String()+"fs> {\"name\":\"SD-Card\",\"attached\":false}");
            }
            break;
        }
        if (this->argList->at(0) == ".exit") {
            this->env->std_out->println("fs> exiting." + cStateInfo);
            return 1; // exit loop
        }
        if (this->argList->size() < 2) { this->env->std_out->println("fs> required 2 arguments" + cStateInfo); break; }
        if (this->argList->at(0) == ".open") {
            if (this->argList->size() < 3) {
                this->env->std_out->println("fs> '.open' required 2 additional arguments" + cStateInfo);
                break;
            }
            auto fr = uFS.open(this->argList->at(1), this->argList->at(2));
            if (!fr.ok) {
                this->env->std_out->println("fs> file could not be opened" + cStateInfo);
                break;
            }
            this->cFile = fr.file;
            this->env->std_out->println("fs> ok" + cStateInfo);
            this->fileOpen = true;
            break;
        }
        if (this->argList->at(0) == ".write") {
            if (!this->fileOpen) {
                this->env->std_out->println("fs> no file open" + cStateInfo);
                break;
            }
            this->fileSizeToWrite = atoll(this->argList->at(1).c_str());
            this->fileSizeWritten = 0;
            this->state = 2; // write file
            this->env->std_out->println("fs> ok" + cStateInfo);
            break;
        }
        this->env->std_out->println("fs> command not found" + cStateInfo);
        break;
    }
    case 2: // write file
    {
        // continue without output
        if (input == "") break;
        int bytesWritten = this->cFile.write(input.c_str());
        this->fileSizeWritten += bytesWritten;
        if (this->fileSizeWritten >= this->fileSizeToWrite) {
            this->state = 0; // read line
        }
        this->env->std_out->println(String()+"fs> Wrote " + bytesWritten + "bytes (finished: " + (this->state == 0) + ") (state 2/write file)");
        break;
    }
    case 3: // read file
        // run as fast/often as possible
        this->nextRun(0);
        // try to write the (perhaps) unwritten character
        if (this->unwritten != -1) {
            if (this->env->std_out->write(this->unwritten)) {
                this->unwritten = -1;
            } else {
                this->nextRun(0.01);
                break;
            }
        }

        while (true) {
            if (!this->cFile.available()) {
                this->state = 0; // read line
                break;
            }
            c = this->cFile.read();
            if (!this->env->std_out->write(c)) {
                this->unwritten = c;
                // buffer propably full; try again later
                this->nextRun(0.01);
                break;
            }
            // timeout after 20ms
            if (procStartMillis < millis() - 20) break;
        }
        break;
    default:
        break;
    }

    return 0;
}