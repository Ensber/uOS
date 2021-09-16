#include "definitions.hpp"

#include <vector>

#ifdef USE_WIFI
    #include <ESP8266WiFi.h>
    #include <ESP8266WiFiMulti.h>
    #include <WiFiClient.h>
    ESP8266WiFiMulti WiFiMulti;
#endif

#include "core/_includeCore.hpp"
#include "drivers/_includeDrivers.hpp"
#include "programs/_includePrograms.hpp"

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("[INFO] Booting...");

    // INIT filesystem Components

    if (uFS.beginInternalFs()) Serial.println("[INFO] Mounted Internal-FS at '/'");
    else                       Serial.println("[WARN] Failed to mount Internal-FS at '/'");

    initDrivers();  // setup all drivers
    initPrograms(); // setup all programs
    
    // needs to be done, before a program can run. 
    std::vector<String>* args = os::programs.parseArgumentList("-port 0");
    devices::serial* serial = new devices::serial(args);
    delete args;

    if (!serial->initialized)
        Serial.println(String()+"[WARN] Error while initialing the serial interface: "+serial->error);

    pEnv env = pEnv(serial);

    // DRIVER INITIALIZATION

    #ifdef USE_LORA // init lora
        program::shell::execute(env, "driver init lora -f 433000000 -pCS 15 -pRST 5 -pIRQ 4");
    #endif
    // program::shell::execute(env, "driver init udp -lp 23 -rIP 192.168.2.110");

    delay(100);

    Serial.println("[INFO] Mounting SD-Card...");
    if (uFS.attachSD("/sd/", 2)) Serial.println("[INFO] Mounted SD-Card at '/sd/'");
    else                         Serial.println("[WARN] Failed to mount SD-Card at '/sd/'");
    
    Serial.println("[INFO] System ready");
    Serial.flush();

    // debug code

    for (int i=0;i<devices::deviceList.size(); i++) {
        Serial.print(String()+i+" - ");
        Serial.println(devices::deviceList[i]->UID);
    }

    // debug code for finding out how memory works
    int mem = ESP.getFreeHeap();

    //program::shell::execute(env, "wifi");

    int mem2 = ESP.getFreeHeap();

    Serial.println(String("AVAILABLE MEM BEFORE: ")+mem);
    Serial.println(String("AVAILABLE MEM AFTER : ")+mem2);
    Serial.println(String("Difference: ")+(mem-mem2));

    Serial.println("before ps exec");
    program::shell::execute(env, "ps");
    Serial.println("after ps exec");
    //for (int i=0;i<30;i++)

    // program::shell::execute(env, "shell");

}

void loop() {
    uint64_t ne = globalEventloop.run();
    if (ne > micros64()) {
        ne = ne - micros64();

        // dirty fix; TODO for later
        if (ne > 1000000) {
            // Serial.println(String()+"[WARN] Long delay periode: "+(unsigned long)(ne/1000000)+"s");
            // Serial.println(         "[WARN] Shortened to 0.01s");
            ne = 10000;
        }

        delay(ne/1000);
    }
}