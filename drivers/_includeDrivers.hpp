#ifndef _INCLUDEDRIVERS_HPP
#define _INCLUDEDRIVERS_HPP

#include "deviceManager.hpp"
#include "deviceManager.cpp"

#include "null.hpp"
#include "null.cpp"

#ifdef USE_LORA
    #include "lora/lora.hpp"
    #include "lora/lora.cpp"
    #include "lora/loraMgr.hpp"
    #include "lora/loraMgr.cpp"
#endif

#include "serial.hpp"
#include "serial.cpp"

#ifdef USE_WIFI
    // #include "udp.hpp"
    // #include "udp.cpp"
#endif

void initDrivers() {
    #ifdef USE_LORA
        program::loraMgr::init();
    #endif
}

#endif