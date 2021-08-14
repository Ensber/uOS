#ifndef DEVICEMANAGER_HPP
#define DEVICEMANAGER_HPP

#include "../interfaces/IODevice.hpp"
#include "lora.hpp"

#include <vector>

namespace devices {
    std::vector<I_IODevice*> deviceList;
    
    // get a device by its uid
    I_IODevice* get(String UID);
}


#endif