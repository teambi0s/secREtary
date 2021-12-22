#include "pin.H"
#include <iostream>
#include <fstream>

using namespace std;

string invalid = "$";
ADDRINT ptrace_addr;


VOID ptrace_patch(VOID *original_routine_ptr, int *return_address){
  __asm__(".intel_syntax noprefix;"
      "nop;"
      ".att_syntax;"
      );
}


INT32 Usage(){
    cerr << "This tool finds&patches ptrace call." << endl << endl;
    cerr << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}


VOID FindPtrace(INS ins, VOID *v){
	if(INS_IsCall(ins)){
		cout << "0x" << hex <<INS_Address(ins) << "  ";
		cout << INS_Disassemble(ins) << endl;
		//if ptrace found
		//ld_linux_attach.cpp
		//PIN_Detach(); this does the patching;
	}
}

VOID Image(IMG current_image,VOID *v){
	// do patchwork here;
    // look for the routine in the loaded image
    RTN current_routine = RTN_FindByName(current_image, "ptrace");

    // stop if the routine was not found in this image
    if (!RTN_Valid(current_routine))
        return;

    // skip routines which are unsafe for replacement
    if (!RTN_IsSafeForProbedReplacement(current_routine)){
        std::cerr << "Skipping unsafe routine " << "ptrace" << " in image " << IMG_Name(current_image) << std::endl;
        return;
    }

    // replacement routine's prototype: returns void, default calling standard, name, takes no arugments
    PROTO replacement_prototype = PROTO_Allocate(PIN_PARG(void), CALLINGSTD_DEFAULT, "ptrace", PIN_PARG_END());

    // replaces the original routine with a jump to the new one
    RTN_ReplaceSignatureProbed(current_routine,
                               AFUNPTR(ptrace_patch),
                               IARG_PROTOTYPE,
                               replacement_prototype,
                               IARG_ORIG_FUNCPTR,
                               IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                               IARG_RETURN_IP,
                               IARG_END);

    PROTO_Free(replacement_prototype);

	cout << "Done Patching" <<endl;
}

VOID Fini(INT32 code, VOID *v){
	cerr << "Done Execution " << "Ptrace at " << hex << ptrace_addr << endl;
}

int main(int argc, char *argv[]){
	PIN_InitSymbols();
    
	if(PIN_Init(argc,argv))
		return Usage();

	IMG_AddInstrumentFunction(Image,0);
	
	PIN_AddFiniFunction(Fini,0);
	
	//PIN_AddDetachFunction(runpatch, 0);
	
	//PIN_StartProgram();

    //start program in probe mode
    PIN_StartProgramProbed();

	return 0;
}
