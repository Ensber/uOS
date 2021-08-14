#ifndef SHELL_HPP
#define SHELL_HPP

#include "../libs/LinkedList.h"
#include "../interfaces/Task.hpp"

namespace program {
    class shell {
        private:
        public:
            class process : public I_Task {
                public:
                    process(pEnv* env):I_Task(env){};
                    int run();
                    ~process();
            };
            static void init();
            static int execute(pEnv env, String command);
            static int main(pEnv env, std::vector<String>* args);
        protected:
            static String version;
    };
}

#endif