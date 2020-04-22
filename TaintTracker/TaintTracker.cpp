#include "pin.H"
#include "Taint.h"
#include <asm/unistd.h>
#include <fstream>
#include <iostream>
#include <list>

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
int TaintTracker::addTaint(UINT64, UINT64) {
    // Add basic tainting routine here
    return 0;
}

int TaintTracker::removeTaint(UINT64, UINT64) {
    // Removes Taint
    return 0;
}

bool TaintTracker::checkTaint(UINT64) {
    // Returns whether a memory is tainted or not
    return false;
}
/*
 * END OF TAINT TRACKER FUNCTIONS
 */

/*
 * PIN MODULE FUNCTIONS
 */
VOID Trace(TRACE trace, VOID *v) {
    // Instruction Iterator
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
        for ( INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins)) {
            /*
             * Here we need to iterate over all the instruction and note
             *  1. All the move instructions handling tainted memory
             *  2. All the cmp intstructions handling tainted memory
             *  3. Call instructions maybe
             */
            if (INS_Opcode(ins) == XED_ICLASS_CMP) {

            } else {
                if (INS_Opcode(ins) == XED_ICLASS_MOV) {

                } else {
                
                }
            }
        }
    }
}


void Syscall_entry(THREADID thread_id, CONTEXT *ctx, SYSCALL_STANDARD std, void *v) {
    if (PIN_GetSyscallNumber(ctx, std) == __NR_read) {

        int start = static_cast<UINT64>((PIN_GetSyscallArgument(ctx, std, 1)));
		int size  = static_cast<UINT64>((PIN_GetSyscallArgument(ctx, std, 2)));
        
        /* TODO: Backtrace to check if currently in a library function
        * Steps:
        *  1. Figure out all possible library calls that might call read - avoid obscure ones
        *  2. Need to find out how to retrive the arguments (shall we keep a track of all calls)
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