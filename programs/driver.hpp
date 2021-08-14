#ifndef DRIVER_HPP
#define DRIVER_HPP

#include "../core/pEnv.hpp"

#include <vector>

namespace program {
    class driver {
        public:
            static void init();
            static int main(pEnv env, std::vector<String>* args);
        protected:
            static String version;
    };
}

#endif