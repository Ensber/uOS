// fs.hpp

#ifndef fs_HPP
#define fs_HPP

#include "../interfaces/Task.hpp"
#include "../definitions.hpp"
#include "../core/fileSystem.hpp"

#include <vector>

namespace program {
    class fs {
        private:
        public:
            class process : public I_Task {
                private:
                    File cFile;
                    String line;
                    long long int fileSizeWritten;
                    long long int fileSizeToWrite;
                    bool fileOpen = false;
                    int unwritten = -1;
                    std::vector<String>* argList = nullptr;
                public:
                    process(pEnv* env) : I_Task(env){};
                    int run();
                    ~process();
            };
            static void init();
            static int main(pEnv env, std::vector<String>* args);
        protected:
            static String version;
    };
}

#endif