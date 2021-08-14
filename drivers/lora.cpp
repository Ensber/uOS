#include <SPI.h>
#include <LoRa.h>
#include "../core/misc.hpp"
#include "lora.hpp"

namespace devices {
    int lora::uidCounter = 0;

    lora::lora(std::vector<String>* arguments) {
        this->type = "lora";

        auto res  = findArg(arguments, "-f");
        if (!res.found) {this->error = "[-f] no frequency set"; return;}
        long frequency = atol(res.arg.c_str());

        res  = findArg(arguments, "-pCS");
        if (!res.found) {this->error = "[-pCS] no chip select set"; return;}
        int pCS = atoi(res.arg.c_str());

        res  = findArg(arguments, "-pRST");
        if (!res.found) {this->error = "[-pRST] no chip reset set"; return;}
        int pRST = atoi(res.arg.c_str());

        res  = findArg(arguments, "-pIRQ");
        if (!res.found) {this->error = "[-pIRQ] no chip interrupt set"; return;}
        int pIRQ = atoi(res.arg.c_str());

        this->loraDevice = new LoRaClass();
        this->loraDevice->setPins(pCS, pRST, pIRQ);
        if (!this->loraDevice->begin(frequency)) {
            this->error = "cannot begin lora module"; return;
        }

        // commit the module to the device list, IF it initialized correctly
        initialized = true;
        this->UID = this->type + this->uidCounter++;
        deviceList.push_back(this);
    }

    bool lora::set(String key, void* value) {
        if (key == "freq") {
            this->loraDevice->setFrequency(*(long*)value);
            return true;
        }
        if (key == "bw") {
            this->loraDevice->setSignalBandwidth(*(long*)value);
            return true;
        }
        return false;
    }

    int lora::parsePacket() {return this->loraDevice->parsePacket();}
    int lora::available() {return this->loraDevice->available();}
    char lora::read() {return this->loraDevice->read();}
    char* lora::readManny(int size){
        char* outArr = new char[size + 1];
        for (byte i=0; i<size; i++) outArr[i] = this->loraDevice->read();
        outArr[size] = 0;
        return outArr;
    }
    int lora::beginPacket() {return this->loraDevice->beginPacket();}
    int lora::beginPacket(bool implicid) {return this->loraDevice->beginPacket(implicid);}
    int lora::write(byte b) {return this->loraDevice->write(b);}
    int lora::write(byte* buffer, int size) {return this->loraDevice->write(buffer, size);}
    void lora::endPacket() {this->loraDevice->endPacket();}
    void lora::endPacket(bool async) {this->loraDevice->endPacket(async);}
}