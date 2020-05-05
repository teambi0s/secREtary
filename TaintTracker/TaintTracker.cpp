#include "pin.H"
#include "Taint.h"
#include <asm/unistd.h>
#include <fstream>
#include <iostream>
#include <list>
/*
 * BASE VARIABLE TO CHECK LIBC
 */
#define LIBC_BASE 0x70000000000

/* BASIC TAINT AREA STRUCTURE */
struct range
{
  UINT64 start;
  UINT64 end;
};

std::list<struct range> bytesTainted;

/*




 * BASIC UTILITY FUNCTIONS
 */
int usage()
{
    cerr << "Usage: ./TaintTracker file" << endl;
    return -1;
}

bool isLibraryFunction( UINT64 startAddr) {
    if (startAddr >= LIBC_BASE ){
      return true;
    }
    else {
      return false;
    }
}

/*
 * END OF BASIC UTILITY FUNCTIONS
 */

/*
 * TAINT TRACKER FUNCTIONS
 */
int TaintTracker::addTaint(UINT64, UINT64) {
    // Add basic tainting routine here
    bytesTainted.push_back(taint);

    return 0;
}

int TaintTracker::removeTaint(UINT64, UINT64) {
    // Removes Taint
    list<struct range>::iterator i;
    bytesTainted.remove(addr);
    return 0;
}

bool TaintTracker::checkTaint(UINT64) {

    // Returns whether a memory is tainted or not
    bool check_taint = false;
    list<struct range>::iterator i;
    struct range taint;

    for(i = bytesTainted.begin(); i != bytesTainted.end(); i++) {
				if (((struct range) *i).start <= addr && ((struct range) *i).size + ((struct range) *i).start >= addr) {
						check_taint = true;
						break;
				}
		}
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
              INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)cmpInst,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_MEMORYOP_EA, 0,
                IARG_END);

            }
            else {
              if (INS_Opcode(ins) == XED_ICLASS_MOV) {

            }
            else {

                }
            }
        }
    }
}


void Syscall_entry(THREADID thread_id, CONTEXT *ctx, SYSCALL_STANDARD std, void *v) {
    if (PIN_GetSyscallNumber(ctx, std) == __NR_read) {

      struct range taint;

      taint.start = static_cast<UINT64>((PIN_GetSyscallArgument(ctx, std, 1)));
	    taint.end  = static_cast<UINT64>((PIN_GetSyscallArgument(ctx, std, 2)));
      bytesTainted.push_back(taint);
      addTaint(taint)   //not sure

        INS head = BBL_InsHead(bbl)
        if ( isLibraryFunction(head) ){
          std::cout << "READ syscall made from libc function" ;
        }



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
 /*
  * NEVER RETURNS
  */
    PIN_StartProgram();

    return 0;
}
/*
 * END OF PIN MODULE FUNCTIONS
 */
