#include <stdio.h>
#include <sys/syscall.h>

#include "syscallbasic.h"
#include "SyscallLog.h"
#include "pin.H"
#include <stack>
#include <iostream>


#define LIBC_BASE 0x700000000000

SyscallLog taintEngine(256);

FILE * trace;

bool isLibraryFunction(UINT64 addr) {
    // TODO : Temporary hack
    // Figure out to do this

    // We can get the name of the fucntion like this
    // if its any help in figuring this out

    //string name = RTN_FindNameByAddress(addr);

    if (addr > LIBC_BASE) {
        return true;
    }
    return false;
}


VOID Image(IMG img, VOID *v){
    //printf ("   image_lowAddress    =  \n",image_lowAddress);
    bool isMainExecutable = IMG_IsMainExecutable(img);
    if (isMainExecutable == true){
            ADDRINT image_lowAddress   = IMG_LowAddress(img);
            printf (" [ + ] Image starts at address    = 0x%zx \n", image_lowAddress);
    }

}

void showstack(stack <function> s)
{
    fprintf(trace, "\t[START BACKTRACE]\n");
    fprintf(trace, "\t-----------------\n");
    while (!s.empty())
    {
        fprintf(trace, "\t0x%lx %s\n", s.top().address, s.top().name.c_str());
        s.pop();
    }
    fprintf(trace, "\t-----------------\n");
    fprintf(trace, "\t[ END BACKTRACE ]\n");
}

void SyscallLog::callFunction(UINT64 insAddr, std::string insDis, UINT64 addr)
{
    function func;
    func.address = addr;
    func.name = RTN_FindNameByAddress(addr);
    taintEngine.callStack.push(func);
}

void SyscallLog::retFunction(UINT64 insAddr, std::string insDis)
{
    taintEngine.callStack.pop();
}

VOID SysBefore(ADDRINT ip, ADDRINT num, ADDRINT arg0, ADDRINT arg1, ADDRINT arg2, ADDRINT arg3, ADDRINT arg4, ADDRINT arg5)
{
    long unsigned int* ret = (long unsigned int*)__builtin_extract_return_addr((long unsigned int*)__builtin_return_address(0));
    function func;
    func.name = RTN_FindNameByAddress((ADDRINT)ret);
    switch (syscall_args[(int)num]) {
    case 0:
        fprintf(trace,"0x%lx: %s()",
            (unsigned long)ip,
            syscall_names[(long)num].c_str());
        break;
    case 1:
        fprintf(trace,"0x%lx: %s(0x%lx)",
            (unsigned long)ip,
            syscall_names[(long)num].c_str(),
            (unsigned long)arg0);
        break;
    case 2:
        fprintf(trace,"0x%lx: %s(0x%lx, 0x%lx)",
            (unsigned long)ip,
            syscall_names[(long)num].c_str(),
            (unsigned long)arg0,
            (unsigned long)arg1);
        break;
    case 3:
        fprintf(trace,"0x%lx: %s(0x%lx, 0x%lx, 0x%lx)",
            (unsigned long)ip,
            syscall_names[(long)num].c_str(),
            (unsigned long)arg0,
            (unsigned long)arg1,
            (unsigned long)arg2);
        break;
    case 4:
        fprintf(trace,"0x%lx: %s(0x%lx, 0x%lx, 0x%lx, 0x%lx)",
            (unsigned long)ip,
            syscall_names[(long)num].c_str(),
            (unsigned long)arg0,
            (unsigned long)arg1,
            (unsigned long)arg2,
            (unsigned long)arg3);
        break;
    case 5:
        fprintf(trace,"0x%lx: %s(0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx)",
            (unsigned long)ip,
            syscall_names[(long)num].c_str(),
            (unsigned long)arg0,
            (unsigned long)arg1,
            (unsigned long)arg2,
            (unsigned long)arg3,
            (unsigned long)arg4);
        break;
    case 6:
        fprintf(trace,"0x%lx: %s(0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx)",
            (unsigned long)ip,
            syscall_names[(long)num].c_str(),
            (unsigned long)arg0,
            (unsigned long)arg1,
            (unsigned long)arg2,
            (unsigned long)arg3,
            (unsigned long)arg4,
            (unsigned long)arg5);
        break;
    default:
        break;
    }
}

VOID SysAfter(ADDRINT ret)
{
    fprintf(trace,"\t = 0x%lx\n", (unsigned long)ret);
    fflush(trace);
    showstack(taintEngine.callStack);
    fflush(trace);
}

VOID SyscallEntry(THREADID threadIndex, CONTEXT *ctxt, SYSCALL_STANDARD std, VOID *v)
{
    SysBefore(PIN_GetContextReg(ctxt, REG_INST_PTR),
        PIN_GetSyscallNumber(ctxt, std),
        PIN_GetSyscallArgument(ctxt, std, 0),
        PIN_GetSyscallArgument(ctxt, std, 1),
        PIN_GetSyscallArgument(ctxt, std, 2),
        PIN_GetSyscallArgument(ctxt, std, 3),
        PIN_GetSyscallArgument(ctxt, std, 4),
        PIN_GetSyscallArgument(ctxt, std, 5));
}

VOID SyscallExit(THREADID threadIndex, CONTEXT *ctxt, SYSCALL_STANDARD std, VOID *v)
{
    SysAfter(PIN_GetSyscallReturn(ctxt, std));
}

// VOID Instruction(INS ins, VOID *v)
// {
//     if (INS_IsSyscall(ins) && INS_HasFallThrough(ins))
//         {
//             INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SysBefore),
//                            IARG_INST_PTR, IARG_SYSCALL_NUMBER,
//                            IARG_SYSARG_VALUE, 0, IARG_SYSARG_VALUE, 1,
//                            IARG_SYSARG_VALUE, 2, IARG_SYSARG_VALUE, 3,
//                            IARG_SYSARG_VALUE, 4, IARG_SYSARG_VALUE, 5,
//                            IARG_END);

//             INS_InsertCall(ins, IPOINT_AFTER, AFUNPTR(SysAfter),
//                            IARG_SYSRET_VALUE,
//                            IARG_END);
//         }
//     if (INS_Disassemble(ins).rfind("call") == 0) {

//         INS_InsertCall(
//                        ins, IPOINT_BEFORE,(AFUNPTR)taintEngine.callFunction,
//                        IARG_ADDRINT, INS_Address(ins),
//                        IARG_PTR, new string(INS_Disassemble(ins)),
//                        IARG_BRANCH_TARGET_ADDR,
//                        IARG_END);
//     }
//     if (INS_Disassemble(ins).rfind("ret") == 0 && !isLibraryFunction(INS_Address(ins))) {
//         INS_InsertCall(
//                        ins, IPOINT_BEFORE,(AFUNPTR)taintEngine.retFunction,
//                        IARG_ADDRINT, INS_Address(ins),
//                        IARG_PTR, new string(INS_Disassemble(ins)),
//                        IARG_END);
//     }
// }


void Trace(TRACE trace, VOID *v) {
    // Instruction Iterator
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl);
         bbl = BBL_Next(bbl)) {
        for ( INS ins = BBL_InsHead(bbl); INS_Valid(ins);
              ins = INS_Next(ins)) {

            if (INS_IsSyscall(ins) && INS_HasFallThrough(ins))
                {
                    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SysBefore),
                                   IARG_INST_PTR, IARG_SYSCALL_NUMBER,
                                   IARG_SYSARG_VALUE, 0, IARG_SYSARG_VALUE, 1,
                                   IARG_SYSARG_VALUE, 2, IARG_SYSARG_VALUE, 3,
                                   IARG_SYSARG_VALUE, 4, IARG_SYSARG_VALUE, 5,
                                   IARG_END);

                    INS_InsertCall(ins, IPOINT_AFTER, AFUNPTR(SysAfter),
                                   IARG_SYSRET_VALUE,
                                   IARG_END);
                }
            if (INS_Disassemble(ins).rfind("call") == 0) {

                INS_InsertCall(
                               ins, IPOINT_BEFORE,(AFUNPTR)taintEngine.callFunction,
                               IARG_ADDRINT, INS_Address(ins),
                               IARG_PTR, new string(INS_Disassemble(ins)),
                               IARG_BRANCH_TARGET_ADDR,
                               IARG_END);
            }
            if (INS_Disassemble(ins).rfind("ret") == 0) {
                INS_InsertCall(
                               ins, IPOINT_BEFORE,(AFUNPTR)taintEngine.retFunction,
                               IARG_ADDRINT, INS_Address(ins),
                               IARG_PTR, new string(INS_Disassemble(ins)),
                               IARG_END);
            }
        }
    }
}

VOID Fini(INT32 code, VOID *v)
{
    fprintf(trace,"#eof\n");
    fclose(trace);
}

INT32 Usage()
{
    PIN_ERROR("This tool prints a log of system calls"
                + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

int main(int argc, char *argv[])
{
    PIN_InitSymbols();
    if (PIN_Init(argc, argv)) return Usage();
    PIN_SetSyntaxIntel();

    trace = fopen("strace.out", "w");

    IMG_AddInstrumentFunction(Image, 0);
    PIN_AddSyscallEntryFunction(SyscallEntry, 0);
    PIN_AddSyscallExitFunction(SyscallExit, 0);
    TRACE_AddInstrumentFunction(Trace, 0);

    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();

    return 0;
}
