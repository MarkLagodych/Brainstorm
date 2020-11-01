#include "brainstorm.h"

void readFile(cmdline_t commandLine) {
    string name;
    commandLine >> name;
    clearCommandLine(commandLine);

    string data;
    ifstream file(name);

    if(file.is_open()) {
        string line;
        while(getline(file, line)) {
            data += line;
            data += '\n';
        }
        file.close();
    }
    data += '\0';
    setCommandLineContent(commandLine, data);
}

void writeFile(cmdline_t commandLine) {
    string name, data;
    commandLine >> name;

    data = commandLineToString(commandLine);
    clearCommandLine(commandLine);
    ptr_t quote = data.find('"');
    ptr_t length = data.size();
    data = data.substr(quote + 1, length - 2 - quote);

    ofstream file(name);
    file << data;
    file.close();
}

void changeConsoleColor(cmdline_t commandLine) {
    #ifdef _WIN32
        map<string, unsigned char> colors = {
            {"black",       0x0},
            {"darkblue",    0x1},
            {"darkgreen",   0x2},
            {"darkcyan",    0x3},
            {"darkred",     0x4},
            {"darkmagenta", 0x5},
            {"darkyellow",  0x6},

            {"grey",        0x7},
            {"darkgrey",    0x8},

            {"blue",        0x9},
            {"green",       0xA},
            {"cyan",        0xB},
            {"red",         0xC},
            {"magenta",     0xD},
            {"yellow",      0xE},
            {"white",       0xF}
        };

        string fg, bg; // foreground, background color names
        commandLine >> fg >> bg;

        auto fgcolor = colors.find(fg), bgcolor = colors.find(bg);
        if(fgcolor != colors.end() && bgcolor != colors.end()) {
            SetConsoleTextAttribute (
                GetStdHandle(STD_OUTPUT_HANDLE),
                bgcolor->second << 4 | fgcolor->second
            );
        }
    #else
        map<string, int> fgcolors = {
            {"black",      30},
            {"red",        31},
            {"green",      32},
            {"yellow",     33},
            {"blue",       34},
            {"magenta",    35},
            {"cyan",       36},
            {"white",      37},
        };

        map<string, int> bgcolors = {
            {"black",      40},
            {"red",        41},
            {"green",      42},
            {"yellow",     43},
            {"blue",       44},
            {"magenta",    45},
            {"cyan",       46},
            {"white",      47},
        };

        string fg, bg; // foreground, background color names
        commandLine >> fg >> bg;

        auto fgcolor = fgcolors.find(fg), bgcolor = bgcolors.find(bg);

        if(fgcolor != fgcolors.end() && bgcolor != bgcolors.end())
            cout << "\033[" << fgcolor->second << ";" << bgcolor->second << "m";
    #endif

    clearCommandLine(commandLine);
}


template<class mem_t>
void BrainstormInterpreter<mem_t>::setCommand(string name, exe_t executor) {
    executors[name] = executor;
}

template<class mem_t>
void BrainstormInterpreter<mem_t>::delCommand(string name) {
    executors.erase(name);
}

template<class mem_t>
void BrainstormInterpreter<mem_t>::exeCommand() {
    string command;
    commandLine >> command;

    auto executor = executors.find(command);

    if(executor != executors.end())
        (*executor->second)(commandLine);
}



template<class mem_t>
BrainstormInterpreter<mem_t>::BrainstormInterpreter() {
    setCommand("fread", &readFile);
    setCommand("fwrite", &writeFile);
    setCommand("color", &changeConsoleColor);
}

template<class mem_t>
BrainstormInterpreter<mem_t>::~BrainstormInterpreter() {
    if(cmd) delete[] cmd;
    if(arg) delete[] arg;
    if(mem) delete[] mem;
    if(src) delete[] src;
}


template<class mem_t>
int BrainstormInterpreter<mem_t>::loadFile(string fileName) {

    ifstream file(fileName, ios::binary);

    if(!file.is_open())
        return EXIT_FAILURE;

    file.seekg(0, file.end);
    ssize = file.tellg();
    file.seekg(0, file.beg);
    src = new char[ssize];
    file.read(src, ssize);
    file.close();

    return EXIT_SUCCESS;
}


template<class mem_t>
void BrainstormInterpreter<mem_t>::loadCStr(const char *source) {
    ptr_t len; // Source code length
    for(len = 0; source[len]; len++);

    src = new char[len];
    for( ; len; len--)
        src[len] = source[len];

}


template<class mem_t>
int BrainstormInterpreter<mem_t>::compile() {
    vector<cmd_t> _cmd;
    vector<ptr_t> _arg;

    ptr_t u = 0; // Pointer in resulting code, not in the source

    stack<ptr_t> lefts; // Left braces positions

    for(ptr_t i = 0; i < ssize; ) {
        switch(src[i]) {
            case '%': {
                i++;
                for( ; i < ssize; i++) {
                    if(src[i] == '%') {
                        i++;
                        break;
                    }
                }
                break;
            }

            case '+': case '-':
            case '.': case ',':
            case '<': case '>':
            case '?': case '!':
            case '$': case '&': {
                _cmd.push_back(src[i]);

                // Compute command amount - read next characters
                //     until another character will be found
                ptr_t j = i;
                char ch = src[i];
                for( ; i < ssize; i++) {
                    if(src[i] != ch) break;
                }
                _arg.push_back(i-j);
                u++;
                break;
            }

            case '[': {
                // It is [-]
                if(ssize >= 3)
                    if(i+2 < ssize)
                        if(src[i+1] == '-' && src[i+2] == ']') {
                            _cmd.push_back('0');
                            _arg.push_back(0);
                            i += 3;
                            u++;
                            break;
                        }

                // It is not [-]
                _cmd.push_back(src[i]);
                _arg.push_back(0);
                lefts.push(u);
                i++;
                u++;
                break;
            }

            case ']': {
                if(!lefts.empty()) {
                    _cmd.push_back(src[i]);
                    _arg.push_back(lefts.top()+1);
                    _arg[lefts.top()] = u+1;
                    /*
                        +1 because
                        for instance, we have a[bc]d
                        we are reading ] and current cell value is nonzero
                        we want to go ] -> b, not ] -> [
                    */
                    lefts.pop();
                    u++;
                }
                i++;
                break;
            }

            default:
                i++;
            break;
        }
    }

    delete[] src;
    src = nullptr;

    // Inappropriate braces
    if(!lefts.empty())
        return EXIT_FAILURE;

    // Copy everything to raw arrays because they are faster
    csize = _cmd.size();
    cmd = new cmd_t[csize];
    arg = new ptr_t[csize];
    for(ptr_t i = 0; i < csize; i++) {
        cmd[i] = _cmd[i];
        arg[i] = _arg[i];
    }

    return EXIT_SUCCESS;
}

template<class mem_t>
void BrainstormInterpreter<mem_t>::allocMem(ptr_t memSize) {
    msize = memSize;
    mem = new mem_t[msize];
    for(register ptr_t i=0; i<msize; i++)
        mem[i] = 0;
}

template<class mem_t>
void BrainstormInterpreter<mem_t>::run() {

    if(mem == nullptr || cmd == nullptr || arg == nullptr)
        return;

    ptr_t mp = 0; // memory pointer
    for(register ptr_t cp = 0; cp < csize; ) { // code pointer

        // All commands are sorted by frequency of occurence
        switch(cmd[cp]) {

            case '>': { mp += arg[cp];                                              cp++; break; }
            case '<': { mp -= arg[cp];                                              cp++; break; }

            case '+': { mem[mp] += arg[cp];                                         cp++; break; }
            case '-': { mem[mp] -= arg[cp];                                         cp++; break; }

            case '[': { if (!mem[mp])         cp = arg[cp];           else          cp++; break; }
            case ']': { if ( mem[mp])         cp = arg[cp];           else          cp++; break; }

            case '0': { mem[mp] = 0;                                                cp++; break; }

            case '$': { commandLineMode = true;                                     cp++; break; }
            case '&': { commandLineMode = false;                                    cp++; break; }


            case '.': {
                if(commandLineMode)
                    if(!mem[mp])
                        exeCommand();
                    else
                        for(ptr_t j = arg[cp]; j; j--)
                            commandLine << (unsigned char) mem[mp];
                else
                    for(ptr_t j = arg[cp]; j; j--)
                        cout << (unsigned char) mem[mp];
                cp++;
                break;
            }

            case ',': {
                unsigned char n;

                if(commandLineMode)
                    for(ptr_t j = arg[cp]; j; j--)
                        commandLine >> n;
                else
                    for(ptr_t j = arg[cp]; j; j--)
                        cin >> n;

                mem[mp] = (mem_t) n;

                cp++;
                break;
            }

            case '!': {
                if(commandLineMode)
                    for(ptr_t j = arg[cp]; j; j--)
                        commandLine << (unsigned long) mem[mp];
                else
                    for(ptr_t j = arg[cp]; j; j--)
                        cout << (unsigned long) mem[mp];
                cp++;
                break;
            }

            case '?': {
                unsigned long n;

                if(commandLineMode)
                    for(ptr_t j = arg[cp]; j; j--)
                        commandLine >> n;
                else
                    for(ptr_t j = arg[cp]; j; j--)
                        cin >> n;

                mem[mp] = (mem_t) n;

                cp++;
                break;
            }

        } // switch(cmd[cp])

    } // for(register ptr_t cp = 0; cp < csize; )

} // void Brainstorm<mem_t>::run()
