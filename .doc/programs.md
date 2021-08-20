# Overview
The programs are stored in the `programs/` directory in the code base. A uOS Program is just a function, wich takes in a Vector of string Arguments. It cannot be a long operation or it will halt the other tasks. The way around it is by spawning a task. A task is a state mashine, wich can be run periodicly.

# A simple Program example

> PRO-TIP: the code can be copied from below and replacing `YOUR_PROGRAM_NAME` will do all the naming work for you

## code
```C++
// YOUR_PROGRAM_NAME.hpp

#ifndef YOUR_PROGRAM_NAME_HPP
#define YOUR_PROGRAM_NAME_HPP

#include "../definitions.hpp"

#include <vector>

namespace program {
    class YOUR_PROGRAM_NAME {
        private:
        public:
            static void init();
            static int main(pEnv env, std::vector<String>* args);
        protected:
            static String version;
    };
}

#endif
```

In the program header file, you will need to implement a `static int main(pEnv, std::vector<String>)` and a kind of init function, in this case `static void init()` wich will add the program registry. This function must later be called by `initPrograms()` in the `programs/_includePrograms.hpp`.

```C++
// YOUR_PROGRAM_NAME.cpp

#include "../core/programRegistry.hpp"
#include "YOUR_PROGRAM_NAME.hpp"

bool __YOUR_PROGRAM_NAME_INIT = false;
void program::YOUR_PROGRAM_NAME::init() {
    // only init the YOUR_PROGRAM_NAME program once
    if (__YOUR_PROGRAM_NAME_INIT) return;
    __YOUR_PROGRAM_NAME_INIT = true;

    os::programs.addProgram("YOUR_PROGRAM_NAME", &program::YOUR_PROGRAM_NAME::main);
}

String program::YOUR_PROGRAM_NAME::version = "\nYOUR_PROGRAM_NAME 0.1.0";

int program::YOUR_PROGRAM_NAME::main(pEnv env, std::vector<String>* args) {
    env.std_out->println(program::YOUR_PROGRAM_NAME::version);
}
```

`__YOUR_PROGRAM_NAME_INIT` is not nescessary, but is a nice to have, to not accidentaly pollute the program registry.
`os::programs.addProgram("YOUR_PROGRAM_NAME", &program::YOUR_PROGRAM_NAME::main);` will add the program with the name `YOUR_PROGRAM_NAME` to the program registry.

`I/O` is done by using the `env` (enviroment) variable. It will give you three pointers to IO Devices.
`std_in`, `std_out` and `std_err` allow you to read and write, but please use them only as they were intendet.



# A simple Program with Task example

```C++
// YOUR_PROGRAM_NAME.hpp

#ifndef YOUR_PROGRAM_NAME_HPP
#define YOUR_PROGRAM_NAME_HPP

#include "../interfaces/Task.hpp"
#include "../definitions.hpp"

#include <vector>

namespace program {
    class YOUR_PROGRAM_NAME {
        private:
        public:
            class process : public I_Task {
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
```

Like in the first example, we create a class, but this time, we are also creating a class inside of our class, wich is a implementation of `I_Task`. It needs to implement a `int run()` wich, at best, is a state mashine.

```C++
#include "../core/programRegistry.hpp"
#include "../core/task.hpp"
#include "YOUR_PROGRAM_NAME.hpp"

bool __YOUR_PROGRAM_NAME_INIT = false;
void program::YOUR_PROGRAM_NAME::init() {
    // only init the YOUR_PROGRAM_NAME program once
    if (__YOUR_PROGRAM_NAME_INIT) return;
    __YOUR_PROGRAM_NAME_INIT = true;

    os::programs.addProgram("YOUR_PROGRAM_NAME", &program::YOUR_PROGRAM_NAME::main);
}

String program::YOUR_PROGRAM_NAME::version = "\nYOUR_PROGRAM_NAME 0.1.0";

int program::YOUR_PROGRAM_NAME::main(pEnv env, std::vector<String>* args) {
    if (env.std_out->lockedBy == 0)
        env.std_out->println(program::YOUR_PROGRAM_NAME::version);

    // create a task
    pEnv* envPtr = new pEnv(env);
    program::YOUR_PROGRAM_NAME::process* proc = new program::YOUR_PROGRAM_NAME::process(envPtr);
    proc->name = "YOUR_PROGRAM_NAME";
    globalEventloop.add(proc);
}

// TASK CODE //

int program::YOUR_PROGRAM_NAME::process::run() {
    this->nextRun(0.1);

    // make sure to only run, if the env is not occupied
    if (this->isEnvLocked()) return 0;

    // run even fewer times than usual. if your cycle tykes up to much time,
    // lets say > 50ms, than you should split it more up or skip the cycle entirely
    if (REQ_LOW_PERFORMANCE) {
        this->nextRun(0.5);
        // return 0; // skip
    }

    // this example will read from the enviroment std_in and execute its contents
    if (!this->env->std_in->available()) return 0;

    String input = "";
    while (this->env->std_in->available()) {
        input += this->env->std_in->read();
    }

    // remove newline at the end of the input
    input = input.substring(0,input.length()-2);
    this->env->std_out->println(">>> "+input);
    program::shell::execute(*env,input);
}
```

`this->nextRun(0.1);` is used to tell the dispatcher, when the soonest time should be, to call your function. 
The funcion may be called later, due to other processes, taking up more time. Again, you can avaoid most of this by having a more split-up state mashine, wich often gives back controll to the dispatcher and other processes. 
For a call on the next cycle, you can either just not call this function or call it with a zero. But keep in mind, that doing this too ofthen will increase the time needed for the dispatcher to run each second. This is just wasted resources. To avaoid this, try to keep the cycletime at around 20ms, but at a maximum of 50ms.
> IF A PROGRAM TAKES TOO LONG, TO EXECUTE A CYCLE, IT MAY BE TERMINATED IN FUTURE VERSIONS OF uOS!!!

Also simmilar to the other example, we are creating a Program function and in it, we are now also creating a new Task/process. by calling `globalEventloop.add(proc);` we are adding it to the event loop and it will start running in the next loop iteration, wich is most times almost instantainous.

By running `if (this->isEnvLocked()) return 0;` we are protecting other processes, wich need to be the only one talking to the interface. I recommend to skip a cycle or just the code, wich would read or write to/from the interfaces.

The `REQ_LOW_PERFORMANCE` lets us know, if another process is requesting some more precessing time for heavier workloads or a precise timing. If the bool s `true` then only do the bare minimum. Also use it only when nescsary, because it could impact other processes.

