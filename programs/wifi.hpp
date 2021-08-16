#ifndef WIFI_PROGRAM_HPP
#define WIFI_PROGRAM_HPP

#include "../interfaces/Task.hpp"

#include <vector>

namespace program {
    class wifi {
        private:
        public:
            class process : public I_Task {
                public:
                    static bool isRunning;
                    process(pEnv* env) : I_Task(env){};
                    int run();
            };
            static void init();
            static int main(pEnv env, std::vector<String>* args);
        protected:
            static String version;
    };
}

#endif