#include "pin.H"
using std::string;
#include <vector>

#define LIBC_BASE 0x700000000000
struct args{
    ADDRINT arg1;
    ADDRINT arg2;
};

struct Images{
    string name;
    ADDRINT lowaddr;
    ADDRINT higaddr;
};

struct Func
{
    string name;
    ADDRINT address;
    int count;
    bool lib;
    std::vector<args> args_list;
    
};
