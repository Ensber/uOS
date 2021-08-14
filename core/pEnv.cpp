#include "pEnv.hpp"

void pEnv::setStdIO(I_IODevice* device) {
    this->std_in  = device;
    this->std_out = device;
    this->std_err = device;
}

pEnv::pEnv(I_IODevice* device) {
    this->setStdIO(device);
}