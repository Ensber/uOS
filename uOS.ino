#include "definitions.hpp"
#include "core/_includeCore.hpp"
#include "drivers/_includeDrivers.hpp"
#include "programs/_includePrograms.hpp"

#include <vector>

struct a
{
    String test;
};

struct b
{
    a* aLink;
};



void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("Booting...");

    initDrivers();  // setup all drivers
    initPrograms(); // setup all programs
    
    // needs to be done, before a program can run. 
    std::vector<String>* args = os::programs.parseArgumentList("-port 0");
    devices::serial* serial = new devices::serial(args);
    delete args;
    if (!serial->initialized)
        Serial.println(String()+"Error while initialing the serial interface: "+serial->error);
    pEnv env = pEnv(serial);

    // init lora
    
    program::shell::execute(env, "driver init lora -f 433000000 -pCS 15 -pRST 5 -pIRQ 4");
    Serial.println("after lora init");

    // debug code

    serial->write((byte*)"hello from the API\n",19);

    I_IODevice* device = devices::get("lora0");
    if (device != nullptr)
        Serial.println(device->type);
    else
        Serial.println("device not found");

    for (int i=0;i<devices::deviceList.size(); i++) {
        Serial.print(String()+i+" - ");
        Serial.println(devices::deviceList[i]->UID);
    }

    // debug code for finding out how memory works
    int mem = ESP.getFreeHeap();

    std::vector<String>* x = new std::vector<String>;
    x->push_back("test");
    x->at(0);

    // std::vector<String>* x = new std::vector<String>;
    // x->push_back("test");
    int mem2 = ESP.getFreeHeap();

    Serial.println(String("AVAILABLE MEM BEFORE: ")+mem);
    Serial.println(String("AVAILABLE MEM AFTER : ")+mem2);
    Serial.println(String("Difference: ")+(mem-mem2));

    // while (1)
    // ESP.wdtFeed();

    program::shell::execute(env, "shell");
    program::shell::execute(env, "shell");
    program::shell::execute(env, "shell");
    program::shell::execute(env, "shell");
    program::shell::execute(env, "ps");
}

void loop() {
    uint64_t ne = globalEventloop.run();
    if (ne > micros64()) {
        ne = ne - micros64();
        delay(ne/1000);
    }
}