#include "../core/programRegistry.hpp"
#include "../core/task.hpp"
#include "../drivers/deviceManager.hpp"
#include "wifi.hpp"

bool __wifi_INIT = false;
void program::wifi::init() {
    // only init the wifi program once
    if (__wifi_INIT) return;
    __wifi_INIT = true;

    os::programs.addProgram("wifi", &program::wifi::main);
}

String program::wifi::version = "\nwifi 0.1.0";

int program::wifi::main(pEnv env, std::vector<String>* args) {
    env.std_out->println(program::wifi::version);

    if (args->size() < 3) {
        env.std_err->println("usage: wifi add <wifi name> <wifi password>");
        return 0;
    }

    // wifi connect "Eika-WLAN-745900" "4776708386864076"
    if (args->at(0) == "add") {
        WiFi.mode(WIFI_STA);
        WiFiMulti.addAP(args->at(1).c_str(), args->at(2).c_str());
        pEnv* envPtr = new pEnv(env);
        program::wifi::process* proc = new program::wifi::process(envPtr);
        proc->isRunning = true;
        proc->name = "wifi connect status";
        globalEventloop.add(proc);
    }
}

// TASK CODE //

bool program::wifi::process::isRunning = false;

int program::wifi::process::run() {
    this->nextRun(1);
    
    // make sure to only run, if the env is not occupied
    if (this->isEnvLocked()) return 0;

    if (WiFiMulti.run() == WL_CONNECTED) {
        program::wifi::process::isRunning = false;
        this->env->std_out->print(String()+"Connected! IP: ");
        IPAddress ip = WiFi.localIP();
        for (int i=0; i<4; i++) {
            this->env->std_out->print(String(ip[i])+'.');
        }
        this->env->std_out->write('\n');
        return 1; // commit suicide
    } else {
        this->env->std_out->println("Connecting...");
    }
    return 0;
}