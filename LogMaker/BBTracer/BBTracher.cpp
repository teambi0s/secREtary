#include <iostream>
#include <fstream>
#include "pin.H"
using std::cerr;
using std::ofstream;
using std::ios;
using std::string;
using std::endl;

#define OFFSET 0xfff
ofstream OutFile;
struct Binary
{
    string name;
    ADDRINT base_addr;
};
std::vector<ADDRINT> bbl_offsets;
ADDRINT baddr;
Binary bin;
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "inscount.out", "specify output file name");
KNOB<ADDRINT> KnobBaseAddress(KNOB_MODE_WRITEONCE, "pintool", "baddr", "0", "specify binary's base address");

VOID docount(ADDRINT addr) 
{ 
    ADDRINT offset = (addr-bin.base_addr);
    if(offset < OFFSET)
    {
        if(baddr == 0){
            baddr = bin.base_addr;
        }
        bbl_offsets.push_back(baddr+offset);
    }
}

VOID Image(IMG img, VOID *v)
{    
    bool isMainExecutable = IMG_IsMainExecutable(img);
    if (isMainExecutable == true)
    {
            bin.base_addr = IMG_LowAddress(img);
            string name = IMG_Name(img);
            std::size_t idx = name.find_last_of("/\\");
            bin.name = name.substr(idx+1);
    }
}
    
VOID Trace(TRACE trace, VOID *v)
{
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR)docount, IARG_ADDRINT, BBL_Address(bbl), IARG_END);
    }
}

VOID Fini(INT32 code, VOID *v)
{    
    OutFile.setf(ios::showbase);
    OutFile<<"[*] Binary name: "<<bin.name<<endl;
    OutFile<<"[*] Binary loaded at address: "<<std::hex<<bin.base_addr<<endl;
    OutFile<<"[*] Given base address for binary<default: loading addr>: "<<std::hex<<baddr<<endl;
    OutFile<<"----- Basic Blocks address executed ------"<<endl;
    for(auto &itr : bbl_offsets){
        OutFile<<"> "<<std::hex<<itr<<endl;
    }
    OutFile.close();
}

INT32 Usage()
{
    cerr << "This tool prints the address of the basic blocks executed" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

int main(int argc, char * argv[])
{
    if (PIN_Init(argc, argv)) return Usage();
    OutFile.open(KnobOutputFile.Value().c_str());
    baddr = KnobBaseAddress.Value();
    IMG_AddInstrumentFunction(Image, 0);
    TRACE_AddInstrumentFunction(Trace, 0);
    PIN_AddFiniFunction(Fini, 0);
    PIN_StartProgram();
    return 0;
}
