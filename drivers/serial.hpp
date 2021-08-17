#ifndef DRV_SERIAL_HPP
#define DRV_SERIAL_HPP

#include "../interfaces/IODevice.hpp"

#include <vector>

namespace devices { 
    class serial : public I_IODevice {
        public:
            // custom start //
            HardwareSerial* serialDevice;
            // custom end //

            serial(std::vector<String>* arguments);

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
            int   println(String str);
            int   print(String str);
            // void  endPacket();
            // void  endPacket(bool async);

        private:
            static int uidCounter;
    };
}

#endif