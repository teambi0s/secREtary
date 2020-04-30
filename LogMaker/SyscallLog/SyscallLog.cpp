#include <stdio.h>
#include <sys/syscall.h>

#include "syscallbasic.h"
#include "pin.H"

FILE * trace;

VOID SysBefore(ADDRINT ip, ADDRINT num, ADDRINT arg0, ADDRINT arg1, ADDRINT arg2, ADDRINT arg3, ADDRINT arg4, ADDRINT arg5)
{
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

VOID Instruction(INS ins, VOID *v)
{
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
    if (PIN_Init(argc, argv)) return Usage();

    trace = fopen("strace.out", "w");

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddSyscallEntryFunction(SyscallEntry, 0);
    PIN_AddSyscallExitFunction(SyscallExit, 0);

    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}