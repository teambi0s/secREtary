#include <iostream>
#include <fstream>
#include <vector>
#include <string.h> 
#include "pin.H"

using std::cerr;
using std::ios;
using std::string;
using std::endl;

#define LIBC_BASE 0x700000000000

std::vector<int> switch_jmps;
std::vector<int> jmp_ins;
ADDRINT image_base;

VOID printSwitch(ADDRINT ip, CONTEXT *ctxt){ 
    ADDRINT val;
    PIN_GetContextRegval(ctxt, LEVEL_BASE::REG_RAX, reinterpret_cast<UINT8*>(&val));
    val = val - image_base;
    if (val > 50){
        switch_jmps.push_back(val);
    }
}

VOID Trace(TRACE trace, VOID *v)
{
    // Visit every basic block  in the trace
    INS tail;
    ADDRINT adr;
    for(BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        tail = BBL_InsTail(bbl);
        adr = INS_Address(tail);
        
        // Here we fetch every block and disassemble the tail instruction 
        if((INS_Disassemble(tail).rfind("jmp rax") == 0) && (adr < LIBC_BASE)){
            INS_InsertPredicatedCall(tail,IPOINT_BEFORE,(AFUNPTR)printSwitch,IARG_INST_PTR,IARG_CONST_CONTEXT,IARG_END);
            jmp_ins.push_back((adr - image_base));
        }
    }
}

VOID Image(IMG img, VOID *v){
    
    bool isMainExecutable = IMG_IsMainExecutable(img);
    
    if (isMainExecutable == true){
            image_base   = IMG_LowAddress(img);
            printf ("[ + ] Image starts at address    = 0x%zx \n", image_base);
    }

}

VOID Fini(INT32 code, VOID *v)
{
    std::map<int, int> Switch_offsets;
    
    printf ("[+] VM switch case found at offset : 0x%x \n", jmp_ins.rbegin()[1]);
    printf ("[+] Number of VM instructions executed : 0x%lx \n", switch_jmps.size());
    for (int i = 0; switch_jmps[i]; i++) { 
        if (Switch_offsets.find(switch_jmps[i]) == Switch_offsets.end()) { 
            Switch_offsets[switch_jmps[i]] = 1; 
        }  
        else { 
            Switch_offsets[switch_jmps[i]]++; 
        } 
    }
    for (auto& it : Switch_offsets) { 
        printf ("[+] Number of times switch case offset 0%x was hit -> 0x%d\n",it.first,it.second);
    } 
}

INT32 Usage()
{
    cerr << "This tool counts the number of dynamic instructions executed" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

int main(int argc, char * argv[])
{
    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();
    
    //Find the base address
    IMG_AddInstrumentFunction(Image, 0);
    // Register Instruction to be called to instrument instructions
    TRACE_AddInstrumentFunction(Trace, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}