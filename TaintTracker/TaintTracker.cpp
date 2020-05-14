#include "pin.H"
#include <asm/unistd.h>
#include <fstream>
#include <iostream>
#include <list>
/*
 * BASE VARIABLE TO CHECK LIBC
 */
ADDRINT LIBC_BASE;

/* BASIC TAINT AREA STRUCTURE */
struct range
{
  UINT64 start;
  UINT64 end;
};

std::list<struct range> bytesTainted;

#include "TaintTracker.h"

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
void cmpInst(INS ins){

}

void movInst(INS ins){

}

void callInst(INS ins){

}
/*
 * END OF BASIC UTILITY FUNCTIONS
 */

/*
 * TAINT TRACKER FUNCTIONS
 */
int TaintTracker::addTaint(UINT64 start, UINT64 size) {
    // Add basic tainting routine here
<<<<<<< HEAD
    bytesTainted.push_back(taint);

=======
    taint.start = start;
    size = (size > taint_max) ? taint_max : size;
    taint.end = taint.start + size;
    bytesTainted.push_back(taint);

    std::cout << "[TAINT]\t\t\tbytes tainted from " << std::hex << "0x"
              << taint.start << " to 0x" << taint.end << " (via read)"
              << std::endl;
>>>>>>> f2be814016fe24aa45e6ac939c93954ca0b48611
    return 0;
}

int TaintTracker::removeTaint(UINT64, UINT64) {
    // Removes Taint
    list<struct range>::iterator i;
    bytesTainted.remove(addr);
    return 0;
}

<<<<<<< HEAD
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
=======
bool TaintTracker::checkTaint(UINT64 addr) {
    // Returns whether a memory is tainted or not

    list<struct range>::iterator i;

    for(i = bytesTainted.begin(); i != bytesTainted.end(); ++i) {
        if (addr >= i->start && addr < i->end) {
            return true;
        }
    }
>>>>>>> f2be814016fe24aa45e6ac939c93954ca0b48611
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
<<<<<<< HEAD

VOID getbase(IMG img){

	LIBC_BASE = IMG_LowAddress(img);

}

VOID Trace(TRACE trace, VOID *v) {
    // Instruction Iterator
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
      for ( INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins)) {
            /*
=======
void Trace(TRACE trace, VOID *v) {
    // Instruction Iterator
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl);
         bbl = BBL_Next(bbl)) {
        for ( INS ins = BBL_InsHead(bbl); INS_Valid(ins);
              ins = INS_Next(ins)) {

            //  Temporary handling of instuctions that calls debug functions

            /* TODO:
>>>>>>> f2be814016fe24aa45e6ac939c93954ca0b48611
             * Here we need to iterate over all the instruction and note
             *  1. All the move instructions handling tainted memory
             *  2. All the cmp intstructions handling tainted memory
             *  3. Call instructions maybe
             */
<<<<<<< HEAD
=======

            /*
            if (INS_Opcode(ins) == XED_ICLASS_CMP) {
>>>>>>> f2be814016fe24aa45e6ac939c93954ca0b48611

            if (INS_Disassemble(ins).find("cmp") != std::string::npos && !isLibraryFunction(INS_Address(ins))) {
              
				INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)cmpInst,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_MEMORYOP_EA, 0,
                IARG_END);

            }
            else if(INS_Disassemble(ins).find("mov") != std::string::npos && !isLibraryFunction(INS_Address(ins))) {
				INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)movInst,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_MEMORYOP_EA, 0,
                IARG_END);

            }
            else if(INS_Disassemble(ins).find("call") != std::string::npos && !isLibraryFunction(INS_Address(ins))){
				INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)callInst,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_MEMORYOP_EA, 0,
                IARG_END);

<<<<<<< HEAD
=======
                } else {

>>>>>>> f2be814016fe24aa45e6ac939c93954ca0b48611
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

<<<<<<< HEAD
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
=======
        UINT64 start = static_cast<UINT64>(PIN_GetSyscallArgument(ctx, std, 1));
        UINT64 size  = static_cast<UINT64>(PIN_GetSyscallArgument(ctx, std, 2));

        /* TODO: Backtrace to check if currently in a library function
>>>>>>> f2be814016fe24aa45e6ac939c93954ca0b48611
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
<<<<<<< HEAD
	IMG_AddInstrumentFunctio(getbase, 0)
	PIN_AddSyscallEntryFunction(Syscall_entry, 0);
=======
    PIN_AddSyscallEntryFunction(Syscall_entry, 0);
>>>>>>> f2be814016fe24aa45e6ac939c93954ca0b48611
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
