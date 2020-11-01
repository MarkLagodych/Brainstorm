/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    Brainstorm                             *
 *                  by Mark Lagodych                         *
 *                                                           *
 * Brainstorm is a Brainf*ck extension                       *
 * New: ! - write number (numeric analog of .)               *
 *      ? - read  number (numeric analog of ,)               *
 *      %...% - comment (+-<>[],.!?$& are ignored inside)    *
 *      $ - switch input/output to a virtual command line    *
 *      & - switch i/o back to console window                *
 *                                                           *
 * Full list:  <  >  +  -  [  ]  .  ,  !  ?  %%  $  &        *
 *                                                           *
 * Some optimisation ideas: https://habr.com/ru/post/321630/ *
 *                                                           *
 * See https://en.wikipedia.org/wiki/Brainfuck               *
 *     https://esolangs.org/wiki/Brainfuck                   *
 *                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef BRAINSTORM_LANG
#define BRAINSTORM_LANG

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <map>

#ifdef _WIN32
    #include <windows.h>
#endif

using namespace std;

typedef unsigned char cmd_t;  // Command
typedef unsigned long ptr_t;  // Pointer
typedef stringstream& cmdline_t; // Virtual command line
typedef void(*exe_t)(cmdline_t); // Virtual command executor


inline void clearCommandLine(cmdline_t commandLine) {
    commandLine.str(string()); // Clear content
    commandLine.clear();       // Clear flags
}

inline string commandLineToString(cmdline_t commandLine) {
    return commandLine.str();
}

inline void setCommandLineContent(cmdline_t commandLine, string content) {
    commandLine.str(content);
}

inline bool commandLineIsEmpty(cmdline_t commandLine) {
    return commandLine.str().empty();
}


template<class mem_t>
class BrainstormInterpreter {
public:

    // For source code
    ptr_t ssize = 0;
    char *src = nullptr;

    // For compiled code
    ptr_t csize = 0; // code size
    cmd_t *cmd = nullptr; // cmd and arg must have same sizes - csize
    ptr_t *arg = nullptr;

    // For memory
    ptr_t msize;  // memory size
    mem_t *mem = nullptr;

    // For queries to a virtual command line
    bool commandLineMode = false;
    stringstream commandLine;

    map<string, exe_t> executors;
    void setCommand(string name, exe_t executor);
    void delCommand(string name);
    void exeCommand();


    // Usual functions

    BrainstormInterpreter();
    ~BrainstormInterpreter();

    void loadCStr(const char* source);
    int  loadFile(string fileName);
    int  compile();
    void allocMem(ptr_t memSize);
    void run();
};

template class BrainstormInterpreter<unsigned char >;
template class BrainstormInterpreter<unsigned short>;
template class BrainstormInterpreter<unsigned long >;

#endif // BRAINSTORM_LANG
