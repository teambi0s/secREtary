
#include <iostream.h>

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
int taint_max;
};

extern TaintTracker taintEngine(100);

