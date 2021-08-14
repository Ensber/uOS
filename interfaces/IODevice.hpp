#ifndef ICOMMS_HPP
#define ICOMMS_HPP


struct _getReturnType {
    bool ok = false;
    String valueType;
    void* value = nullptr;
};

class I_IODevice {
    public:
        String name = "";
        String type = "";
        String UID  = "";
        String error = "";

        bool initialized = false;

        // I_IODevice(std::vector<String>* arguments);

        virtual bool  set(String key, void* value);
        virtual _getReturnType get(String key);

        virtual int   parsePacket();
        virtual int   available() = 0;
        virtual char  read() = 0;
        virtual char* readManny(int size);

        virtual int   beginPacket();
        virtual int   beginPacket(bool implicid);
        virtual int   write(byte byte) = 0;
        virtual int   write(byte buffer[], int size);
        virtual int   println(byte buffer[], int size);
        virtual int   println(String str);
        virtual int   print(String str);
        virtual void  endPacket(){};
        virtual void  endPacket(bool async){};

        virtual bool  open(String location);
        virtual bool  close();
};

// default functions
int I_IODevice::write(byte buffer[], int size) {
    for (int i = 0; i<size; i++) this->write(buffer[i]);
    return size;
}

int I_IODevice::println(byte buffer[], int size) {
    this->write(buffer, size);
    this->write((byte)'\n');
    return size+1;
}

int I_IODevice::println(String str) {
    str += "\n";
    this->write((byte*)str.c_str(), str.length());
    return str.length();
}

int I_IODevice::print(String str) {
    this->write((byte*)str.c_str(), str.length());
    return str.length();
}

char* I_IODevice::readManny(int size) {
    char* outArr = new char[size + 1];
    for (byte i=0; i<size; i++) outArr[i] = this->read();
    outArr[size] = 0;
    return outArr;
}

// default returns
bool I_IODevice::set(String key, void* value){return false;};
_getReturnType I_IODevice::get(String key){return _getReturnType();};
int I_IODevice::parsePacket() {return 0;};
int I_IODevice::beginPacket() {return 0;};
int I_IODevice::beginPacket(bool implicid) {return 0;};
bool I_IODevice::open(String location) {return false;};
bool I_IODevice::close() {return false;};

#endif