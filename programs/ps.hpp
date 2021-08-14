#ifndef PS_HPP
#define PS_HPP

#include "../core/task.hpp"
#include "../interfaces/Task.hpp"

#include <vector>

namespace program {
    class ps {
        private:
        public:
            class process : public I_Task {
                private:
                    struct strippedTaskDataContainer lastDataTelemetry;
                    uint64_t lastTimeCheck;
                public:
                    process(pEnv* env):I_Task(env){};
                    int run();
            };
            static void init();
            static int main(pEnv env, std::vector<String>* args);
        protected:
            static String version;
    };
}

#endif