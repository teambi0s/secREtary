#include "pin.H"
#include <asm/unistd.h>
#include <fstream>
#include <iostream>
#include <list>

class TaintTracker {
public:
    // Constructor and Destructor
    TaintTracker(UINT64 max) : taint_max(max) { }
    ~TaintTracker() { }

    int addTaint(UINT64, UINT64);
    int removeTaint(UINT64, UINT64);
    bool checkTaint(UINT64);

private:
    // We will have the taint variables here
    // Maximum taint size
    UINT64 taint_max;
};
