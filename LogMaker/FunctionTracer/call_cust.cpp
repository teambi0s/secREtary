#include "pin.H"
#include <iostream>
#include <vector>
#include <string.h> 
#include "CallTracer.h"

using std::endl;
using std::cerr;
using std::ios;
using std::string;
using std::stringstream;

bool isMain = 0;
bool isExit = 1;
bool isLib = 0;

ADDRINT image_base;
std::vector<Func> Func_list;
std::map<string, int> countMap;

string invalid = "$";

INT32 Usage()
{
    cerr << "This tool counts the number of dynamic instructions executed" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

VOID Image(IMG img, VOID *v){
    
    bool isMainExecutable = IMG_IsMainExecutable(img);
    
    if (isMainExecutable == true){
            image_base   = IMG_LowAddress(img);
    }
}

//Convert address to function name

const string *Target2String(ADDRINT target)
{
    stringstream tmp_stream;
    string name = RTN_FindNameByAddress(target);

    if (name == ".text"){
        name = "";
    }
    if (name == ""){
        name = "sub_";
        tmp_stream << name << std::hex << target;
        tmp_stream >> name;
        return new string(name);
    }
    else
        return new string(name);
}

VOID do_call(const string *s,ADDRINT target)
{   
    // Here we check for _Exit because that's the last call happening before the binary teminates.
    if (s->rfind("__libc_start_main") == 0){
        isMain = 1;
    }
    if ((s->rfind("exit") == 0) || s->rfind("_exit") == 0){
        isExit = 0;
    }
    if(isMain && isExit){
        if(target > LIBC_BASE){
            isLib = 1;
        }
        else {
            isLib = 0;
        }
        for (auto &itr : Func_list){
            if (itr.name == s->c_str()){
                itr.count++;
                return ;
            }
        }
        switch (s->c_str()[0])
        {
        case '.':
            break;
        case '_':
            break;
        case '$':
            break;
        case '(':
            break;
        case '#':
            break;
        case '@':
            break;
        default:
            Func_list.push_back({s->c_str(),(target - image_base),1,isLib});
        }
    }
}

VOID  do_call_indirect(ADDRINT target, BOOL taken)
{
    if( !taken ) return;

    const string *s = Target2String(target);

    do_call(s,target);

    if (s != &invalid)
        delete s;
}

VOID Trace(TRACE trace, VOID *v)
{   
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        INS tail = BBL_InsTail(bbl);
        //const char * disasm;
        if( INS_IsCall(tail) )
        {
            if( INS_IsDirectControlFlow(tail) )
            {
                ADDRINT target = INS_DirectControlFlowTargetAddress(tail);
                INS_InsertPredicatedCall(tail, IPOINT_BEFORE, AFUNPTR(do_call),
                                            IARG_PTR,Target2String(target),IARG_BRANCH_TARGET_ADDR,IARG_END);
            }
            else
            {
                INS_InsertCall(tail, IPOINT_BEFORE, AFUNPTR(do_call_indirect),
                            IARG_BRANCH_TARGET_ADDR, IARG_BRANCH_TAKEN, IARG_END);
            }
        }
        else
        {
            // sometimes code is not in an image
            RTN rtn = TRACE_Rtn(trace);
            
            // also track stup jumps into share libraries
            if( RTN_Valid(rtn) && !INS_IsDirectControlFlow(tail) && ".plt" == SEC_Name( RTN_Sec( rtn ) ))
            {
                INS_InsertCall(tail, IPOINT_BEFORE, AFUNPTR(do_call_indirect),
                                   IARG_BRANCH_TARGET_ADDR, IARG_BRANCH_TAKEN,IARG_END);
            }
        }
        
    }
}

VOID Fini(INT32 code, VOID *v)
{   
    printf("[+] Analysis Complete\n");
    printf ("[+] Image starts at address    = 0x%zx \n", image_base);
    printf("[+] Function call details\n");
    printf("%-40s%-25s%-20s%-10s\n", "Function Name", "hitCount", "Offset" , "isLibrary");
    for (auto & itr : Func_list){
         printf("%-40s%-25d0x%-20lx%-10d\n", itr.name.c_str(), itr.count, itr.address , itr.lib);
	}
}

int  main(int argc, char *argv[])
{
    
    PIN_InitSymbols();

    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }
    IMG_AddInstrumentFunction(Image, 0);
    TRACE_AddInstrumentFunction(Trace, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns

    PIN_StartProgram();
    
    return 0;
}