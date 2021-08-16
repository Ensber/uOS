int lastMem = 0;
void prf(String x) {
    int mem = ESP.getFreeHeap();
    String memCng = String(mem-lastMem);
    if (memCng[0] != '-') memCng = "+" + memCng;
    Serial.println(String("FREE HEAP '")+x+"' "+mem+" ("+memCng+")");
    lastMem = mem;
}

#define pr(x) prf(x);