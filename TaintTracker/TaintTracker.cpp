#include "pin.H"
#include <asm/unistd.h>
#include <fstream>
#include <iostream>
#include <list>

#include "TaintTracker.h"
TaintTracker taintEngine(256);

/*
 * BASIC UTILITY FUNCTIONS
 */

int usage()
{
    cerr << "Not Properly Used" << endl;
    return -1;
}

bool isLibraryFunction() {
    // TODO : Figure out to do this
    return true;
}
/*
 * END OF BASIC UTILITY FUNCTIONS
 */

/*
 * TAINT TRACKER FUNCTIONS
 */
int TaintTracker::addTaint(UINT64 start, UINT64 size) {
    // Add basic tainting routine here
    taint.start = start;
    size = (size > taint_max) ? taint_max : size;
    taint.end = taint.start + size;
    bytesTainted.push_back(taint);

    std::cout << "[TAINT]\t\t\tbytes tainted from " << std::hex << "0x"
              << taint.start << " to 0x" << taint.end << " (via read)"
              << std::endl;
    return 0;
}

int TaintTracker::removeTaint(UINT64, UINT64) {
    // Removes Taint
    return 0;
}

bool TaintTracker::checkTaint(UINT64 addr) {
    // Returns whether a memory is tainted or not

    list<struct range>::iterator i;

    for(i = bytesTainted.begin(); i != bytesTainted.end(); ++i) {
        if (addr >= i->start && addr < i->end) {
            return true;
        }
    }
    return false;
}
/*
 * END OF TAINT TRACKER FUNCTIONS
 */

/*
 * TAINT TRACKER DEBUG FUNCTIONS
 */
void TaintTracker::readMem(UINT64 insAddr, std::string insDis, UINT64 memOp)
{
    if (taintEngine.checkTaint(memOp)) {
        std::cout << std::hex << "[READ in 0x" << memOp << "]\t"
                  << insAddr << ": " << insDis<< std::endl;
    }
}

void TaintTracker::writeMem(UINT64 insAddr, std::string insDis, UINT64 memOp)
{
    if (taintEngine.checkTaint(memOp)) {
        std::cout << std::hex << "[WRITE in 0x" << memOp << "]\t"
                  << insAddr << ": " << insDis << std::endl;
    }
}

/*
 * END OF DEBUG FUNCTIONS
 */

/*
 * PIN MODULE FUNCTIONS
 */
void Trace(TRACE trace, VOID *v) {
    // Instruction Iterator
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl);
         bbl = BBL_Next(bbl)) {
        for ( INS ins = BBL_InsHead(bbl); INS_Valid(ins);
              ins = INS_Next(ins)) {

            //  Temporary handling of instuctions that calls debug functions

            /* TODO:
             * Here we need to iterate over all the instruction and note
             *  1. All the move instructions handling tainted memory
             *  2. All the cmp intstructions handling tainted memory
             *  3. Call instructions maybe
             */

            /*
            if (INS_Opcode(ins) == XED_ICLASS_CMP) {

            } else {
                if (INS_Opcode(ins) == XED_ICLASS_MOV) {

                } else {

                }
            }
            */
            if (INS_MemoryOperandIsRead(ins, 0) && INS_OperandIsReg(ins, 0)){
                  INS_InsertCall(
                               ins, IPOINT_BEFORE,(AFUNPTR)taintEngine.readMem,
                               IARG_ADDRINT, INS_Address(ins),
                               IARG_PTR, new string(INS_Disassemble(ins)),
                               IARG_MEMORYOP_EA, 0,
                               IARG_END);
            }
            else if (INS_MemoryOperandIsWritten(ins, 0)){
                INS_InsertCall(
                               ins, IPOINT_BEFORE,(AFUNPTR)taintEngine.writeMem,
                               IARG_ADDRINT, INS_Address(ins),
                               IARG_PTR, new string(INS_Disassemble(ins)),
                               IARG_MEMORYOP_EA, 0,
                               IARG_END);
            }
        }
    }
}

void Syscall_entry(THREADID thread_id, CONTEXT *ctx, SYSCALL_STANDARD std,
                   void *v) {
    if (PIN_GetSyscallNumber(ctx, std) == __NR_read) {

        UINT64 start = static_cast<UINT64>(PIN_GetSyscallArgument(ctx, std, 1));
        UINT64 size  = static_cast<UINT64>(PIN_GetSyscallArgument(ctx, std, 2));

        /* TODO: Backtrace to check if currently in a library function
        * Steps:
        *  1. Figure out all possible library calls that might call read
        *  (avoid obscure ones)
        *  2. Need to find out how to retrive the arguments
        *  (shall we keep a track of all calls)
        *  3. Taint only the small chunk of memory
        */

        taintEngine.addTaint(start, size);
    }
}

int main(int argc, char *argv[]) {
    if (PIN_Init(argc, argv)) {
        return usage();
    }

    PIN_SetSyntaxIntel();
    PIN_AddSyscallEntryFunction(Syscall_entry, 0);
    TRACE_AddInstrumentFunction(Trace, 0);

    PIN_StartProgram();

    return 0;
}
/*
 * END OF PIN MODULE FUNCTIONS
 */
