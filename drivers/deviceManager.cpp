#include <vector>

// get device by UID or string if no UID match was found
I_IODevice* devices::get(String UID) {
    int deviceSize = devices::deviceList.size();
    I_IODevice* matchingName = nullptr;
    for (int i=0; i<deviceSize; i++) {
        I_IODevice* cDevice = devices::deviceList[i];
        if (cDevice->name == UID) {
            matchingName = cDevice;
        }
        if (cDevice->UID == UID) {
            return devices::deviceList[i];
        }
    }
    // if no match by UID but by name was found:
    if (matchingName != nullptr && UID != "") {
        return matchingName;
    }
    return nullptr;
}