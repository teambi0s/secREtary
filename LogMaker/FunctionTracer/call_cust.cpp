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


std::vector<string> calls_list;
std::map<string, int> countMap;

string invalid = "$";

INT32 Usage()
{
    cerr << "This tool counts the number of dynamic instructions executed" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

//Convert address to function name

const string *Target2String(ADDRINT target)
{
    stringstream tmp_stream;
    string name = RTN_FindNameByAddress(target);
    if (name == ""){
        name = "sub_";
        tmp_stream << name << std::hex << target;
        tmp_stream >> name;
        return new string(name);
    }
    else
        return new string(name);
}

VOID do_call(const string *s)
{
    // Here we check for _Exit because that's the last call happening before the binary teminates.
    calls_list.push_back(*s);
}

VOID  do_call_indirect(ADDRINT target, BOOL taken)
{
    if( !taken ) return;

    const string *s = Target2String(target);
    do_call(s);
    
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
                                            IARG_PTR,Target2String(target),IARG_END);
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
    string ser;
    for(unsigned int i = 0; i < calls_list.size();i++)
    {
        ser = calls_list[i];
        
        //Switch cases for parsing the list we have 
        switch (ser[0])
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
            auto result = countMap.insert(std::pair<string, int>(ser, 1));
	        if (result.second == false)
	            result.first->second++;
            break;
        }
    }
    printf("[+] Analysis Complete\n");
    if ( std::find(calls_list.begin(), calls_list.end(), "ptrace@plt") != calls_list.end() ) 
    {
        printf("[+] PTRACE detected");
    }
    printf("[+] Function call details\n");
    printf("%-40s%-25s%-5s\n", "Function Name", "Times Called", "Address");
    for (auto & elem : countMap){
    // If frequency count is greater than 1 then its a duplicate element
	if (elem.second >= 1){
        printf("%-40s%-25d%-5s\n", elem.first.c_str(), elem.second, "Address");
	}
    }

}

int  main(int argc, char *argv[])
{
    
    PIN_InitSymbols();

    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }
    
    TRACE_AddInstrumentFunction(Trace, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns

    PIN_StartProgram();
    
    return 0;
}