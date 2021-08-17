#ifndef MISC_HPP
#define MISC_HPP

#include "pEnv.hpp"

#include <vector>

bool uAssert(pEnv env, bool state, String message);
String formatMicrosToTime(uint64_t time);
double dround(double number, int digits);

struct argResponse {
    bool found;
    String arg;
};
argResponse findArg(std::vector<String>* args, String name);

#endif