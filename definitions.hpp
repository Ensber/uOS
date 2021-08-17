#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

/* MANAGE PROGRAMS AND DRIVERS */

#define USE_LORA // LORA module must initialize, before you can init the SD card, if a LORA module is present
#define USE_WIFI

/* *************************** */

// Tasks and programs should be implemented in a way
// that if this state is active, they are using less performance
// or if they are not required, that they dont even run.
// This is usefull, if you want one task to get more processing time
// if, and only if, it really needs it 
bool REQ_LOW_PERFORMANCE = false;

int lastMem = 0;
void prf(String x) {
    int mem = ESP.getFreeHeap();
    String memCng = String(mem-lastMem);
    if (memCng[0] != '-') memCng = "+" + memCng;
    Serial.println(String("FREE HEAP '")+x+"' "+mem+" ("+memCng+")");
    lastMem = mem;
}

#define pr(x) prf(x);

#endif