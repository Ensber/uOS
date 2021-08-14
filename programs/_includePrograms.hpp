#ifndef _INCLUDEPROGRAMS_HPP
#define _INCLUDEPROGRAMS_HPP

#include "shell.hpp"
#include "shell.cpp"

#include "driver.hpp"
#include "driver.cpp"

#include "ps.hpp"
#include "ps.cpp"

void initPrograms() {
    program::shell::init();
    program::driver::init();
    program::ps::init();
}

#endif