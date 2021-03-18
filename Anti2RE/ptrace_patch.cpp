#include "pin.H"
#include <iostream>
#include <fstream>

using namespace std;

string invalid = "$";
ADDRINT ptrace_addr;


INT32 Usage()
{
    cerr << "This tool finds&patches ptrace call." << endl << endl;
    cerr << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

VOID ImageLoad(IMG img, VOID *v)
{
	RTN rtn = RTN_FindByName(img, "ptrace");
	if(RTN_Valid(rtn))
	{
		ptrace_addr = RTN_Address(rtn);
	}
}

VOID FindPtrace(INS ins, VOID *v)
{
	if(INS_IsCall(ins))
	{
		cout << "0x" << hex <<INS_Address(ins) << "  ";
		cout << INS_Disassemble(ins) << endl;
		//if ptrace found
		//ld_linux_attach.cpp
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
	cerr << "Done Execution " << "Ptrace at " << hex << ptrace_addr << endl;
}

int main(int argc, char *argv[])
{
	PIN_InitSymbols();
	if(PIN_Init(argc,argv))
	{
		return Usage();
	}
	IMG_AddInstrumentFunction(ImageLoad, NULL);

	INS_AddInstrumentFunction(FindPtrace, 0);
	
	PIN_AddFiniFunction(Fini,0);
	
	PIN_AddDetachFunction(runpatch, 0);
	
	PIN_StartProgram();

	return 0;
}
