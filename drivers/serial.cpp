#include "deviceManager.hpp"

#include <vector>

namespace devices {
    int serial::uidCounter = 0;

    serial::serial(std::vector<String>* arguments) {
        this->type = "serial";

        auto res  = findArg(arguments, "-port");
        if (!res.found) {this->error = "[-port] no port set"; return;}
        int port = atoi(res.arg.c_str());

        switch (port) {
            case 0: serialDevice = &Serial; break;
            case 1: serialDevice = &Serial1; break;
            default: this->error = "invalid port"; return;
        }

        // commit the module to the device list, IF it initialized correctly
        initialized = true;
        this->UID = this->type + this->uidCounter++;
        deviceList.push_back(this);
    }

    // pass through to the serial device
    int serial::available()
        {return this->serialDevice->available();}
    char serial::read()
        {return this->serialDevice->read();}
    int serial::write(byte c)
        {return this->serialDevice->write(c);}
    int serial::write(byte buffer[], int size)
        {return this->serialDevice->write(buffer, size);}
    int serial::println(String str) 
        {return this->serialDevice->println(str);};
    int serial::print(String str) 
        {return this->serialDevice->print(str);};
}