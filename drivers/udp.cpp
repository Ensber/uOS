#include "deviceManager.hpp"
#include "udp.hpp"

#include <vector>

namespace devices {
    int udpDrv::uidCounter = 0;

    udpDrv::udpDrv(std::vector<String>* arguments) {
        this->type = "udpDrv";

        auto res  = findArg(arguments, "-rport");
        if (!res.found) {this->error = "[-rport] no port set"; return;}
        this->localUdpPort = atoi(res.arg.c_str());

        auto res  = findArg(arguments, "-dport");
        if (!res.found) {this->error = "[-dport] no port set"; return;}
        this->remoteUdpPort = atoi(res.arg.c_str());

        

        // commit the module to the device list, IF it initialized correctly
        initialized = true;
        this->UID = this->type + this->uidCounter++;
        deviceList.push_back(this);
    }

    // pass through to the udpDrv device
    // int udpDrv::available()
    //     {return this->udpDrvDevice->available();}
    // char udpDrv::read()
    //     {return this->udpDrvDevice->read();}
    // int udpDrv::write(byte c)
    //     {return this->udpDrvDevice->write(c);}
    // int udpDrv::write(byte buffer[], int size)
    //     {return this->udpDrvDevice->write(buffer, size);}
    // int udpDrv::println(String str) 
    //     {return this->udpDrvDevice->println(str);};
    // int udpDrv::print(String str) 
    //     {return this->udpDrvDevice->print(str);};
}