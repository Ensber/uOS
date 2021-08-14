#ifndef DRV_NULL_HPP
#define DRV_NULL_HPP

#include "../interfaces/IODevice.hpp"

namespace devices { 
    class nullDrv : public I_IODevice {
        public:
            // custom start //
            HardwareSerial* serialDevice;
            // custom end //

            nullDrv(std::vector<String>* arguments);

            // bool  set(String key, void* value);
            // void* get(String key);

            // int   parsePacket();
            int   available();
            char  read();
            // char* readManny(int bytes);

            // int   beginPacket();
            // int   beginPacket(bool implicid);
            int   write(byte byte);
            int   write(byte buffer[], int size);
            // void  endPacket();
            // void  endPacket(bool async);

        private:
            static int uidCounter;
    };
}

#endif