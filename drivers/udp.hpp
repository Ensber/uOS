#ifndef DRV_udpDrv_HPP
#define DRV_udpDrv_HPP

#include "../interfaces/IODevice.hpp"
#include "../core/misc.hpp"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <vector>

namespace devices { 
    class udpDrv : public I_IODevice {
        public:
            // custom start //
            WiFiUDP *udpDevice;
            unsigned int localUdpPort = 0;
            unsigned int remoteUdpPort = 0;
            String remoteIP;
            // custom end //

            udpDrv(std::vector<String>* arguments);

            bool  set(String key, void* value);
            _getReturnType get(String key);

            int   parsePacket();
            int   available();
            char  read();
            // char* readManny(int bytes);

            int   beginPacket();
            // int   beginPacket(bool implicid);
            int   write(byte byte);
            int   write(byte buffer[], int size);
            int   println(String str);
            int   print(String str);
            void  endPacket();
            // void  endPacket(bool async);
            void  flush();

        private:
            static int uidCounter;
    };
}

#endif