#ifndef _INCLUDEDRIVERS_HPP
#define _INCLUDEDRIVERS_HPP

#include "deviceManager.hpp"
#include "deviceManager.cpp"

#include "null.hpp"
#include "null.cpp"

#ifdef USE_LORA
    #include "lora.hpp"
    #include "lora.cpp"
#endif

#include "serial.hpp"
#include "serial.cpp"

void initDrivers() {
    
}

#endif