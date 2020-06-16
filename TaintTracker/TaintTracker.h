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

class TaintTracker {
public:
    // Constructor and Destructor
    TaintTracker(UINT64 max) : taint_max(max) { }
    ~TaintTracker() { }

    int addTaint(UINT64, UINT64);
    int removeTaint(UINT64, UINT64);
    bool checkTaint(UINT64);

    // Debug Functions
    static void readMem(UINT64, std::string, UINT64);
    static void writeMem(UINT64, std::string, UINT64);
    static void callFunction(UINT64, std::string, UINT64);
    static void retFunction(UINT64, std::string);

    //TODO: make this private maybe?
    std::stack <function> callStack;
private:
    // We will have the taint variables here
    // Maximum taint size
    UINT64 taint_max;
    range taint;
    std::list<struct range> bytesTainted;
};
