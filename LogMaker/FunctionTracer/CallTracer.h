#include "pin.H"
using std::string;

struct Function
{
    string name;
    ADDRINT address;
    int count;
    ADDRINT callee;
    bool lib;
};