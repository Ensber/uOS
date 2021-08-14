#ifndef PENV_HPP
#define PENV_HPP

#include "../interfaces/IODevice.hpp"

class pEnv {
    public:
        String path = "/";
        I_IODevice* std_in;
        I_IODevice* std_out;
        I_IODevice* std_err;

        pEnv(){};
        pEnv(I_IODevice* device);
        
        void setStdIO(I_IODevice* device);
};

#endif