#ifndef PROGRAMREGISTRY_HPP
#define PROGRAMREGISTRY_HPP

#include "pEnv.hpp"

class programRegistry {
    public:
        programRegistry();
        // functionpointer for programs with input of a linked list as Arguments
        typedef int (*functionPtr) (pEnv, std::vector<String>*);

        // program registry
        struct programEntry{
            String name;
            functionPtr function;
        };

        struct returnState {
            int returncode = -1;
            int index = -1;
            bool found = false;
        };

        struct programAlias{
            String from;
            String to;
        };

        std::vector<String>* parseArgumentList(String args); //

        void addProgram(String name, functionPtr program); //
        void removeProgram(String name); //
        returnState runProgram(pEnv env, String name, std::vector<String>* args); //
        returnState runProgram(pEnv env, String name); //
        returnState runProgram(pEnv env, std::vector<String>* args); //

        void addAlias(String from, String to);
        void removeAlias(String from);
    protected:
        std::vector<programEntry> pReg;
        std::vector<programAlias> pAlias;
        std::vector<String> emptyStringList;
};

namespace os {
    // instantiate a programRegistry
    programRegistry programs;
}
#endif