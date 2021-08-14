#ifndef DRV_LORA_HPP
#define DRV_LORA_HPP

#include <SPI.h>
#include <LoRa.h>
#include "../interfaces/IODevice.hpp"
#include "../libs/LinkedList.h"

namespace devices { 
    class lora : public I_IODevice {
        public:
            // custom start //
            LoRaClass * loraDevice;
            // custom end //

            lora(std::vector<String>* arguments); //

            bool  set(String key, void* value);
            //void* get(String key);

            int   parsePacket();
            int   available();
            char  read();
            char* readManny(int bytes);

            int   beginPacket();
            int   beginPacket(bool implicid);
            int   write(byte byte);
            int   write(byte buffer[], int size);
            void  endPacket();
            void  endPacket(bool async);

        private:
            static int uidCounter;
    };
}

#endif