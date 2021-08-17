#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include "../definitions.hpp"
#include "../interfaces/IODevice.hpp"

#include "LittleFS.h"
#include <SPI.h>
#include <SD.h>
#include <vector>

class fileSystem {
    private:
        String sdPath = "";

        struct realFSLink
        {
            bool internal = true;
            String path = "";
        };

        class fileIODevice : public I_IODevice {

        };
        
        realFSLink getRealPath(String path);
    public:
        bool sdCardAttached = false;
        bool internalFsAttached = false;
        
        struct fileResponse {
            bool ok = true;
            File file;
        };

        bool fileOpen = false;

        byte LIST_FILES = 1;
        byte LIST_DRECTORIES = 2;

        bool attachSD(String path, int cs);
        bool detatchSD();
        bool beginInternalFs();

        bool mkDir(String path);
        bool rmDir(String path);
        std::vector<String> lsDir(String path, byte contentType);

        fileResponse open(String path, String mode);
        void close(File file);
        void close(fileResponse file);

        bool remove(String path);

        // rename, show sd path if searched for
};

fileSystem uFS;

#endif