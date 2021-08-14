#include "pEnv.hpp"

bool assert(pEnv env, bool state, String message) {
    if (!state) {
        env.std_err->println(message);
    }
    return state;
}

String formatMicrosToTime(uint64_t time) {
    String out = "";
    String temp = "";

    time /= 100000; // convert to 0.1s
    out = String() + "." + (int)(time % 10);

    time /= 10; // convert to s
    temp = (int)(time % 60);
    if (temp.length() < 2) temp = "0" + temp;
    out = ":" + temp + out;

    time /= 60; // convert to minutes
    temp = (int)(time % 60);
    if (temp.length() < 2) temp = "0" + temp;
    out = ":" + temp + out;

    time /= 60; // convert to hours
    temp = (int)(time % 24);
    if (temp.length() < 2) temp = "0" + temp;
    out = ":" + temp + out;

    time /= 24; // convert to days
    out = (unsigned long)time + out;

    return out;
}

double dround(double number, int digits){
    number *= pow(10,digits);
    return round(number)/pow(10,digits);
}

argResponse findArg(std::vector<String>* args, String name) {
    argResponse out;
    out.found = false;
    for (int i=0; i<args->size(); i++) {
        if (args->operator[](i) == name) {
            out.found = true;
            out.arg = args->operator[](i+1);
        }
    }
    return out;
}