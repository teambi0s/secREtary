#include "pin.H"
#include "TaintTracker.h"
TaintTracker taintEngine(256);

/*
 * BASIC UTILITY FUNCTIONS
 */
#define LIBC_BASE 0x700000000000

int usage()
{
    cerr << "Not Properly Used" << endl;
    return -1;
}

bool isLibraryFunction(UINT64 addr) {
    // TODO : Temporary hack
    // Figure out to do this
    if (addr > LIBC_BASE) {
        return true;
    }
    return false;
}

// TODO: make sure dynamic addreses can be used
// or if we need to rebase the addresses
void showstack(stack <function> s)
{
    std::cout << "[Function Backtrace]" << endl;
    std::cout << "--------------------";
    while (!s.empty())
    {
        std::cout << "\n0x" << s.top().address << " " << s.top().name;
        s.pop();
    }
    std::cout << '\n';
    std::cout << "--------------------" << endl;
    std::cout << "[  END  BACKTRACE  ]" << endl;
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
// TODO: Code for reference, clean up later
// void TaintTracker::callFunctionReg(UINT64 insAddr, std::string insDis, REG reg, CONTEXT * ctx)
// {
//     std::cout << "[CALL] at 0x" << insAddr << "] " << insDis << " calls 0x" << static_cast<UINT64>((PIN_GetContextReg(ctx, REG_FullRegName(reg))))<< std::endl;
// }

void TaintTracker::callFunction(UINT64 insAddr, std::string insDis, UINT64 addr)
{
    //std::cout << "[CALL] at 0x" << insAddr << "] " << insDis << " calls 0x" << addr << std::endl;
    function func;
    func.address = addr;
    func.name = RTN_FindNameByAddress(addr);
    taintEngine.callStack.push(func);
}

void TaintTracker::retFunction(UINT64 insAddr, std::string insDis)
{
    //UINT64 addr = taintEngine.callStack.top();
    //std::cout << "[RETURN] " << insDis << "popped 0x" << addr << endl;
    taintEngine.callStack.pop();
}

void TaintTracker::readMem(UINT64 insAddr, std::string insDis, UINT64 memOp)
{
    if (taintEngine.checkTaint(memOp)) {
        std::cout << std::hex << "[READ in 0x" << memOp << "]\t"
                  << insAddr << ": " << insDis<< std::endl;
        showstack(taintEngine.callStack);
    }
}

void TaintTracker::writeMem(UINT64 insAddr, std::string insDis, UINT64 memOp)
{
    if (taintEngine.checkTaint(memOp)) {
        std::cout << std::hex << "[WRITE in 0x" << memOp << "]\t"
                  << insAddr << ": " << insDis << std::endl;
        showstack(taintEngine.callStack);
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

            // XXX Gives cleaner output, but implementation is incorrect
            if (isLibraryFunction(INS_Address(ins))) {
                continue;
            }

            if (INS_Disassemble(ins).rfind("call") == 0) {
                // TODO: Code for reference, clean up later
                // if (INS_OperandIsReg(ins, 0)) {
                // INS_InsertCall(
                //                ins, IPOINT_BEFORE,(AFUNPTR)taintEngine.callFunctionReg,
                //                IARG_ADDRINT, INS_Address(ins),
                //                IARG_PTR, new string(INS_Disassemble(ins)),
                //                IARG_UINT64, INS_OperandReg(ins, 0),
                //                IARG_CONTEXT, IARG_END);
                // } else if (INS_OperandIsMemory(ins, 0)) {
                // INS_InsertCall(
                //                ins, IPOINT_BEFORE,(AFUNPTR)taintEngine.callFunctionImm,
                //                IARG_ADDRINT, INS_Address(ins),
                //                IARG_PTR, new string(INS_Disassemble(ins)),
                //                IARG_MEMORYOP_EA, 0,
                //                IARG_END);
                // } else {
                INS_InsertCall(
                               ins, IPOINT_BEFORE,(AFUNPTR)taintEngine.callFunction,
                               IARG_ADDRINT, INS_Address(ins),
                               IARG_PTR, new string(INS_Disassemble(ins)),
                               IARG_BRANCH_TARGET_ADDR,
                               IARG_END);
                //}
            }
            if (INS_Disassemble(ins).rfind("ret") == 0 && !isLibraryFunction(INS_Address(ins))) {
                INS_InsertCall(
                               ins, IPOINT_BEFORE,(AFUNPTR)taintEngine.retFunction,
                               IARG_ADDRINT, INS_Address(ins),
                               IARG_PTR, new string(INS_Disassemble(ins)),
                               IARG_END);
            }
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
    // For function names
    PIN_InitSymbols();

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
