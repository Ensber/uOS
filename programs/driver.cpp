#include "../core/programRegistry.hpp"
#include "../drivers/_includeDrivers.hpp"
#include "driver.hpp"
#include "shell.hpp"

bool __DRIVER_INIT = false;
void program::driver::init() {
    // only init the shell program once
    if (__DRIVER_INIT) return;
    __DRIVER_INIT = true;

    os::programs.addProgram("driver", &program::driver::main);
    os::programs.addAlias("init", "driver init");
}

String program::driver::version = "\nDriver-manager 0.1.0";

int program::driver::main(pEnv env, std::vector<String>* args) {
    env.std_out->println(program::driver::version);

    // if no args are provided, open manual
    if (args->size() == 0) {
        program::shell::execute(env, "man driver");
        return 0;
    }

    if (args->size() == 1) {
        env.std_err->println("usage: driver <command> <args>");
        return 1;
    }

    String command = args->at(0);
    String arg = args->at(1);
    args->erase(args->begin(), args->begin() + 1);
    I_IODevice* device = nullptr;
    if (command == "init") {
        if (!uAssert(env, arg != "", "usage: driver start <type>")) return 1;

        // initialize device
        if (arg == "serial")   device = new devices::serial(args);
        if (arg == "null")     device = new devices::nullDrv(args);
        #ifdef USE_LORA
            if (arg == "lora") device = new devices::lora(args);
        #endif
        #ifdef USE_WIFI
            // if (arg == "udp")      device = new devices::udpDrv(args);
        #endif

        if (!uAssert(env, device != nullptr, "device '" + arg + "' not found")) return 1;
        if (!device->initialized) {
            env.std_err->println("Device did not initialize correctly\nError: " + device->error);
            return 1;
        } else {
            env.std_out->println("Device '" + device->UID + "' initialized successfully.");
            return 0;
        }
    }
    if (command == "list" && arg == "devices") {
        env.std_out->println("List of all currently connected devices:");
        for (int i=0;i<devices::deviceList.size(); i++) {
            env.std_out->println(String()+ i + " -> " + devices::deviceList[i]->UID + " [" + devices::deviceList[i]->type + "]");
        }
        return 0;
    }
    return 0;
}