#include "pin.H"
using std::string;

#define LIBC_BASE 0x700000000000

struct Func
{
    string name;
    ADDRINT address;
    int count;
    bool lib;
    //ADDRINT callee;
};