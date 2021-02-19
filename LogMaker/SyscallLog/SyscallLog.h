
#include "pin.H"
#include <asm/unistd.h>
#include <fstream>
#include <iostream>
#include <list>
#include <stack>

struct range {
    UINT64 start;
    UINT64 end;
};

struct function {
    UINT64 address;
    string name;
};

class SyscallLog{
public:
    // Constructor and Destructor
    SyscallLog(UINT64 max) : depth_max(max) { }
    ~SyscallLog() { }
    static void callFunction(UINT64, std::string, UINT64);
    static void retFunction(UINT64, std::string);


    //TODO: make this private maybe?
    std::stack <function> callStack;
private:
    UINT64 depth_max;
};
