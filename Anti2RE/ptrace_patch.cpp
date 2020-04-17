#include "pin.H"
#include <iostream>
#include <fstream>

using namespace std;

string invalid = "$";

INT32 Usage()
{
    cerr << "This tool finds&patches ptrace call." << endl << endl;
    cerr << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

const string *Target2String(ADDRINT target)
{
    string name = RTN_FindNameByAddress(target);
    cout << name << endl;
    if (name == "")
        return &invalid;
    else
        return new string(name);
}

VOID FindPtrace(INS ins, VOID *v)
{
	if(INS_IsCall(ins))
	{
		cout << INS_Disassemble(ins) << endl;
		//if ptrace found
		//PIN_Detach(); this does the patching;
	}
}

VOID runpatch(VOID *v)
{
	//do patchwork here;
	cout << "Done Patching" <<endl;
}
VOID Fini(INT32 code, VOID *v)
{
	cerr << "Done Patching" << endl;
}

int main(int argc, char *argv[])
{
	PIN_InitSymbols();
	if(PIN_Init(argc,argv))
	{
		return Usage();
	}
	INS_AddInstrumentFunction(FindPtrace, 0);
	PIN_AddFiniFunction(Fini,0);
	PIN_AddDetachFunction(runpatch, 0);
	PIN_StartProgram();

	return 0;
}
