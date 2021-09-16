#include "../core/programRegistry.hpp"
#include "../core/task.hpp"
#include "../core/misc.hpp"
#include "../drivers/deviceManager.hpp"
#include "ps.hpp"

bool __PS_INIT = false;
void program::ps::init() {
    // only init the shell program once
    if (__PS_INIT) return;
    __PS_INIT = true;

    os::programs.addProgram("ps", &program::ps::main);
    os::programs.addAlias("kill", "ps kill");
}

String program::ps::version = "\nProcess 0.1.0";

int program::ps::main(pEnv env, std::vector<String>* args) {
    env.std_out->println(program::ps::version);

    Serial.println("@0");

    if (args->size() == 0) {
        Serial.println("@0.0");
        program::ps::process* proc = new program::ps::process(new pEnv(env));
        Serial.println("@0.1");
        proc->name = "ps";
        globalEventloop.add(proc);
        Serial.println("@0.2");
        Serial.println("EXIT @1");
        return 0;
    }

    Serial.println("@1");

    if (args->size() == 1) {
        env.std_err->println("usage: ps <command> <args>");
        Serial.println("EXIT @2");
        return 1;
    }

    Serial.println("@0");

    String command = args->at(0);
    String arg = args->at(1);
    if (command == "kill") {
        int pid = atoi(arg.c_str());
        if(globalEventloop.kill(pid)) {
            env.std_out->println(String()+"Process " + pid + " was terminated");
        } else {
            env.std_out->println(String()+"Process " + pid + " could not be found and/or terminated");
        }
    }

    Serial.println("PS main end");
}


// TASK CODE //

int program::ps::process::run() {
    this->nextRun(1);
    
    // make sure to only run, if the env is not occupied
    if (this->isEnvLocked()) return 0;

    if (REQ_LOW_PERFORMANCE) {
        this->env->std_out->println("[PS] skipping due to performance request");
        this->nextRun(5);
        return 0;
    }

    struct strippedTaskDataContainer cTelemetry = globalEventloop.getTelemetry();
    String wifiStatus;
    String secondLine;
    uint64_t cTime = micros64();
    uint64_t timeDelta = cTime - this->lastTimeCheck;
    uint64_t totalRuntimeUsage;
    double timeMultiplyer = 1/((double)timeDelta/1000000);

    bool cellActive;

    this->env->std_out->println("\nPS LIST");
    int offset = 0;
    int tableWidth;
    int CPU_usageWith = 0;
    double CPU_usage;

    // table header settings
    const int warnCycleTime = 20;
    const int columns = 6;
    const String header[columns] = {"pid","process name","state","time usage total","time usage (ms/s)","in percent"};
    String segLine[columns];

    for (int i=0;i<columns; i++) CPU_usageWith += header[i].length();
    CPU_usageWith += (columns-1) * 3 - 23; // add the " | " spaces and remove the side spaces
    switch (this->state) {
        case 0:
            // gather
            this->state = 1;
            this->env->std_out->println("Analyzing...");
            break;
        case 1:
            // display
            // copy the c telemetry
            struct strippedTaskDataContainer tempTelemetry;
            tempTelemetry.size = cTelemetry.size;
            tempTelemetry.data = new strippedTaskData[cTelemetry.size];
            offset = 0;
            totalRuntimeUsage = 0;
            // this should be somewhat linear in execution time; at least not O^2
            for (int i=0;i<cTelemetry.size;i++){
                // make a temp copy
                tempTelemetry.data[i] = cTelemetry.data[i];
                // loop forward to find the process in the last telemetry dump
                // also wrap around, in case the process got in front somehow...
                for (int j = 0; j < this->lastDataTelemetry.size; j++) {
                    // try this address in the last telemetry
                    int otherPtr = (i + offset + j) % this->lastDataTelemetry.size;
                    if (tempTelemetry.data[i].pid == lastDataTelemetry.data[otherPtr].pid) {
                        // if found, then subtract the current runtime from the last to get
                        // the time it ran during the measurement periode. Also adjust the offset
                        // to search faster
                        uint64_t runtime = tempTelemetry.data[i].runtime - lastDataTelemetry.data[otherPtr].runtime;
                        totalRuntimeUsage += runtime;
                        tempTelemetry.data[i].runtime = runtime * timeMultiplyer;
                        offset = (offset + j) % this->lastDataTelemetry.size;
                        break;
                    }
                }
            }

            // setup colums
            // {"pid","process name","time usage total","time usage (ms)","in percent"}
            unsigned int colSizes[columns];
            for (int i=0;i<columns;i++) {
                colSizes[i] = header[i].length();
            }
            // calculate spacings
            for (int i=0;i<tempTelemetry.size;i++){
                auto cur = tempTelemetry.data[i];
                colSizes[0] = max(colSizes[0],String(cur.pid).length());
                colSizes[1] = max(colSizes[1],cur.name.length());
                colSizes[2] = max(colSizes[2],String(cur.state).length());
                colSizes[3] = max(colSizes[3],formatMicrosToTime(cTelemetry.data[i].runtime).length());
                if (cur.longestCycleTime > warnCycleTime) {
                    colSizes[4] = max(colSizes[4],((String((double)(cur.runtime/10)/100))+cur.longestCycleTime).length() + 3);
                } else {
                    colSizes[4] = max(colSizes[4],(String((double)(cur.runtime/10)/100)).length());
                }
                colSizes[5] = max(colSizes[5],String(round((double)cur.runtime/(double)timeDelta*10000/timeMultiplyer)/100).length()+1);
            }

            // general info
            #ifdef USE_WIFI
                if (WiFi.isConnected()) {
                    IPAddress ip = WiFi.localIP();
                    wifiStatus = String() + ip[0] + "." + ip[1] + "." + ip[2] + "." + ip[3];
                } else {
                    wifiStatus = "unconnected";
                }
                wifiStatus = "                       WIFI         : "  + wifiStatus;
            #endif

            this->env->std_out->println(String("Uptime      : ")+formatMicrosToTime(micros64())+wifiStatus);
            secondLine = String("Free memory : ")+ESP.getFreeHeap();
            for (int i=secondLine.length(); i<49; i++) secondLine += ' ';
            secondLine += String()+"Cycle health : " + round(timeMultiplyer*100)/100;
            this->env->std_out->println(secondLine);
            this->env->std_out->print  (String("CPU Usage   : ["));
            cellActive = true;
            CPU_usage = (double)totalRuntimeUsage/(double)timeDelta;
            for (int i=0;i<CPU_usageWith;i++){
                double cPos = (i+1.00)/(double)CPU_usageWith;
                if (cPos < CPU_usage) {
                    this->env->std_out->write('=');
                } else if (cellActive) {
                    this->env->std_out->write('>');
                    cellActive = false;
                } else {
                    this->env->std_out->write(' ');
                }
            }
            this->env->std_out->println(String("] ") + round(CPU_usage*10000)/100 + "%");

            // table code

            tableWidth = -3;
            for (int i=0;i<columns;i++) tableWidth += colSizes[i] + 3;

            // do it all again and now print it
            for (int i=-1;i<tempTelemetry.size;i++){
                String line = "";
                if (i == -1) {
                    for (int j=0;j<columns;j++) {
                        segLine[j] = header[j];
                    }
                } else {
                    auto cur = tempTelemetry.data[i];
                    segLine[0] = cur.pid;
                    segLine[1] = cur.name;
                    segLine[2] = String(cur.state);
                    segLine[3] = formatMicrosToTime(cTelemetry.data[i].runtime);
                    segLine[4] = (double)(cur.runtime/10)/100;
                    segLine[5] = String() + (round((double)cur.runtime/(double)timeDelta*10000/timeMultiplyer)/100) + "%";
                }
                for (int j=0;j<columns;j++) {
                    this->env->std_out->print(segLine[j]);

                    // fill whitespace to align table
                    int spaces = colSizes[j];
                    bool printLine = false;
                    if (j == 4 && i != -1 && tempTelemetry.data[i].longestCycleTime > warnCycleTime) {
                        line = String(" ")+tempTelemetry.data[i].longestCycleTime+"ms";
                        printLine = true;
                        spaces -= line.length();
                    }

                    // warn if a cycle time is over warnCycleTime
                    for (int k = segLine[j].length(); k < spaces; k++) this->env->std_out->write(' ');

                    if (printLine)
                        env->std_out->print(line);

                    // write table delimiter
                    if (j != columns-1)
                        this->env->std_out->write((byte*)" | ", 3);
                }
                this->env->std_out->write('\n');
                if (i == -1) {
                    for (int j = 0; j < tableWidth; j++) this->env->std_out->write('=');
                    this->env->std_out->write('\n');
                }
            }

            // output
            // for (int i=0;i<tempTelemetry.size;i++){
            //     auto cur = tempTelemetry.data[i];
            //     this->env->std_out->println(String()+cur.pid+": "+cur.name+" T~"+(unsigned long)cur.runtime+" N~"+cur.name);
            // }

            // cleanup
            for (int i=0;i<columns;i++) segLine[i] = "";
            delete[] tempTelemetry.data;
            delete[] this->lastDataTelemetry.data;
            
            break;
        default:
            this->env->std_err->println(String()+"[PS] invalid state: " + this->state);
            break;
    }

    this->lastDataTelemetry = cTelemetry;
    this->lastTimeCheck = cTime;
    return 0;
}