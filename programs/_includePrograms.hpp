#ifndef _INCLUDEPROGRAMS_HPP
#define _INCLUDEPROGRAMS_HPP

#include "shell.hpp"
#include "shell.cpp"

#include "driver.hpp"
#include "driver.cpp"

#include "ps.hpp"
#include "ps.cpp"

#include "wifi.hpp"
#include "wifi.cpp"

void initPrograms() {
    program::shell::init();
    program::driver::init();
    program::ps::init();
    program::wifi::init();
}

#endif