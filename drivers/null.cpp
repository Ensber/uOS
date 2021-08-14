#include "deviceManager.hpp"
#include "null.hpp"

#include <vector>

namespace devices {
    int nullDrv::uidCounter = 0;

    nullDrv::nullDrv(std::vector<String>* arguments) {
        this->type = "null";

        // commit the module to the device list, IF it initialized correctly
        initialized = true;
        this->UID = this->type + this->uidCounter++;
        deviceList.push_back(this);
    }

    int nullDrv::available() {return 0;}
    char nullDrv::read() {return 0;}
    int nullDrv::write(byte c) {return 0;}
    int nullDrv::write(byte buffer[], int size) {return 0;}
}