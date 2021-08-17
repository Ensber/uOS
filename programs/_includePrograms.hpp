#ifndef _INCLUDEPROGRAMS_HPP
#define _INCLUDEPROGRAMS_HPP

#include "shell.hpp"
#include "shell.cpp"

#include "driver.hpp"
#include "driver.cpp"

#include "ps.hpp"
#include "ps.cpp"

#include "fs.hpp"
#include "fs.cpp"

#ifdef USE_WIFI
    #include "wifi.hpp"
    #include "wifi.cpp"
#endif

void initPrograms() {
    program::shell::init();
    program::driver::init();
    program::ps::init();
    #ifdef USE_WIFI
        program::wifi::init();
    #endif
    program::fs::init();
}

#endif