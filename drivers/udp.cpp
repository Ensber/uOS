#include "deviceManager.hpp"
#include "udp.hpp"

#include <vector>

namespace devices {
    int udpDrv::uidCounter = 0;

    udpDrv::udpDrv(std::vector<String>* arguments) {
        this->type = "udpDrv";

        auto res  = findArg(arguments, "-lp");
        if (!res.found) {this->error = "[-lp] no port set"; return;}
        this->localUdpPort = atoi(res.arg.c_str());

        res  = findArg(arguments, "-rp");
        if (!res.found) {this->error = "[-rp] no port set"; return;}
        this->remoteUdpPort = atoi(res.arg.c_str());

        // parse IP address
        res  = findArg(arguments, "-rIP");
        if (!res.found) {this->error = "[-rIP] no IP set"; return;}
        this->remoteIP = res.arg;

        // commit the module to the device list, IF it initialized correctly
        initialized = true;
        this->UID = this->type + this->uidCounter++;
        deviceList.push_back(this);
    }

    // pass through to the udpDrv device
    int udpDrv::beginPacket()
        {return this->udpDevice->beginPacket(this->remoteIP.c_str(), this->remoteUdpPort);}
    void udpDrv::endPacket()
        {this->udpDevice->endPacket();}
    int udpDrv::available()
        {return this->udpDevice->available();}
    char udpDrv::read()
        {if (this->udpDevice->remoteIP().toString() != remoteIP) return 0xFF;return this->udpDevice->read();}
    int udpDrv::write(byte c)
        {return this->udpDevice->write(c);}
    int udpDrv::write(byte buffer[], int size)
        {return this->udpDevice->write(buffer, size);}
    int udpDrv::println(String str) 
        {return this->udpDevice->println(str);};
    int udpDrv::print(String str) 
        {return this->udpDevice->print(str);};
    void udpDrv::flush() 
        {this->udpDevice->flush();};
    int udpDrv::parsePacket()
        {return this->udpDevice->parsePacket();};
}