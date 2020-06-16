#include "pin.H"
#include <asm/unistd.h>
#include <fstream>
#include <iostream>
#include <list>

struct range {
    UINT64 start;
    UINT64 end;
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
private:
    // We will have the taint variables here
    // Maximum taint size
    UINT64 taint_max;
    range taint;
    std::list<struct range> bytesTainted;
};


//extern TaintTracker taintEngine(256);
