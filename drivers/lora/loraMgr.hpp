// loraMgr.hpp

#ifndef loraMgr_HPP
#define loraMgr_HPP

#include "../../interfaces/Task.hpp"
#include "../../definitions.hpp"

#include <vector>

namespace program {
    class loraMgr {
        private:
        public:
            class process : public I_Task {
                private:
                    bool isMaster = false;
                public:
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
