#include "pin.H"
#include "TaintTracker.h"
TaintTracker taintEngine(256);
//<<<<<<< HEAD
//=======

//>>>>>>> 4600b921aa57d8a760d21e6d60fa36e24828baae
/*
 * BASIC UTILITY FUNCTIONS
 */
#define LIBC_BASE 0x700000000000

int usage()
{
    std::cerr << "Not Properly Used" << std::endl;
    return -1;
}

VOID Image(IMG img, VOID *v){
    //printf ("   image_lowAddress    =  \n",image_lowAddress);
    bool isMainExecutable = IMG_IsMainExecutable(img);
    if (isMainExecutable == true){
            ADDRINT image_lowAddress   = IMG_LowAddress(img);
            printf (" [ + ] Main starts at address    = 0x%zx \n", image_lowAddress);
    }

}

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

// TODO: make sure dynamic addreses can be used
// or if we need to rebase the addresses
void showstack(stack <function> s)
{
    std::cout << "[Function Backtrace]" << std::endl;
    std::cout << "--------------------";
    while (!s.empty())
    {
        std::cout << "\n0x" << s.top().address << " " << s.top().name;
        s.pop();
    }
    std::cout << '\n';
    std::cout << "--------------------" << std::endl;
    std::cout << "[  END  BACKTRACE  ]" << std::endl;
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

int TaintTracker::removeTaint(UINT64 start, UINT64 size) {
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
*  CMP FUNCTIONS
*/
VOID cmpRegs(UINT64 insAddr, std::string insDis, REG reg1, REG reg2, CONTEXT * ctx) {
	char buffer1[100];
	char buffer2[100];
		
	if (!REG_valid(reg1) || !REG_valid(reg1)) {
			cout << "[BUG] Invalid registers!! - please report" << endl;
			return ;
	}
	
    cout << "Cmp at 0x" << insAddr << " ]\t " << insDis << endl;
    sprintf(buffer1, "[ Tainted %s ] :\t 0x%016lx { String display coming soon }\n", REG_StringShort(REG_FullRegName(reg1)).c_str(), static_cast<UINT64>((PIN_GetContextReg(ctx, REG_FullRegName(reg1)))));
    cout << buffer1;
    sprintf(buffer2, "[ Operand %s ] :\t 0x%016lx { String display coming soon }\n", REG_StringShort(REG_FullRegName(reg2)).c_str(), static_cast<UINT64>((PIN_GetContextReg(ctx, REG_FullRegName(reg2)))));	
    cout << buffer2 << endl;
}

VOID cmpRegImm(UINT64 insAddr, std::string insDis, REG reg1, UINT64 imm, CONTEXT * ctx) {
	char buffer[100];
		cout << "[Tainted Cmp(Register with Immediate) at 0x" << insAddr << " ]\t :" << insDis << endl;
		    sprintf(buffer, "[ %s ] :\t 0x%016lx { String display coming soon }\n", REG_StringShort(REG_FullRegName(reg1)).c_str(), static_cast<UINT64>((PIN_GetContextReg(ctx, REG_FullRegName(reg1)))));
			cout << buffer;
			// TODO : Handle immediate values better
			cout << "[Immediate Value] : " << imm << endl << endl;
}


VOID cmpMemImm(UINT64 insAddr, std::string insDis, UINT64 memOp, UINT32 size, UINT64 imm, CONTEXT * ctx) {
	//UINT64 addr = memOp;
	//list<UINT64>::iterator i;
	//char * data_region;

	//for (i = addressTainted.begin(); i != addressTainted.end(); i++) {
	//if (addr == *i){
	cout << "[ Cmp(Memory with Immediate) at 0x" << insAddr << "]\t :" << insDis << endl;
	cout << "[Compared Memory] : " << endl;
    //data_region = new char[size];

	//PIN_SafeCopy(data_region, (void *)addr, size);
	//dump_data(addr, size, data_region);

	cout << "[Immediate Value] : " << imm << endl << endl;
	//delete data_region;
	//}
    //}
}

VOID cmpMemReg(UINT64 insAddr, std::string insDis, UINT64 memOp, UINT32 size, REG reg1, CONTEXT * ctx) {
	//UINT64 addr = memOp;
	//bool isMemoryTainted = false;
	//bool isRegTainted = false;
	//list<UINT64>::iterator i;
	//char * data_region;
	char buffer[100];

	//for (i = addressTainted.begin(); i != addressTainted.end(); i++) { 
	//	if (addr == *i){
	//		isMemoryTainted = true;	
	//	}
	//}
	//if (checkAlreadyRegTainted(reg1)) {
	//		isRegTainted = true;
	//}
	//if (isRegTainted || isMemoryTainted) {
	cout << "[Cmp(Memory with Register) at  0x" << insAddr << "]\t : " << insDis << endl;
	sprintf(buffer, "[ %s %s ] :\t 0x%016lx { String display coming soon }\n", "Tainted" , REG_StringShort(REG_FullRegName(reg1)).c_str(), static_cast<UINT64>((PIN_GetContextReg(ctx, REG_FullRegName(reg1)))));
	//sprintf(buffer, "[ %s %s ] :\t 0x%016lx { String display coming soon }\n", ((isRegTainted) ? "Tainted" : "Untainted" ) ,REG_StringShort(REG_FullRegName(reg1)).c_str(), static_cast<UINT64>((PIN_GetContextReg(ctx, REG_FullRegName(reg1)))));
	
    //	data_region = new char[size];

	cout << "[" << "Tainted" << "Memory] : " << endl;
		
	//	PIN_SafeCopy(data_region, (void *)addr, size);
	//	dump_data(addr, size, data_region);
		

	//	delete data_region;
	//}
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

            if (isLibraryFunction(INS_Address(ins))) {
                continue;
            }
            if (INS_Disassemble(ins).rfind("cmp") == 0 && !isLibraryFunction(INS_Address(ins))) {
                
               			if (INS_OperandIsReg(ins, 0) && INS_OperandIsReg(ins, 1)) {
								    INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)cmpRegs,
									            IARG_ADDRINT, INS_Address(ins),
												IARG_PTR, new string(INS_Disassemble(ins)),
												IARG_UINT64, INS_OperandReg(ins, 0),
												IARG_UINT64, INS_OperandReg(ins, 1),
												IARG_CONTEXT, IARG_END);			
								}
						else if (INS_OperandIsReg(ins, 0) && INS_OperandIsImmediate(ins, 1)) {
									INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)cmpRegImm,
												IARG_ADDRINT, INS_Address(ins),
												IARG_PTR, new string(INS_Disassemble(ins)),
												IARG_UINT64, INS_OperandReg(ins, 0),
												IARG_UINT64, INS_OperandImmediate(ins, 1),
												IARG_CONTEXT, IARG_END);			
						}
                
						/*else if (INS_hasKnownMemorySize(ins)) {
											INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)cmpMemImm,
														IARG_ADDRINT, INS_Address(ins),
														IARG_PTR, new string(INS_Disassemble(ins)),
														IARG_MEMORYOP_EA, 0,
														IARG_MEMORYREAD_SIZE,
														IARG_UINT64, INS_OperandImmediate(ins, 1),
														IARG_CONTEXT, IARG_END);			
									}
						else {
											INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)cmpMemImm,
														IARG_ADDRINT, INS_Address(ins),
														IARG_PTR, new string(INS_Disassemble(ins)),
														IARG_MEMORYOP_EA, 0,
														IARG_UINT32, 16,
														IARG_UINT64, INS_OperandImmediate(ins, 1),
														IARG_CONTEXT, IARG_END);			

						} */
						else if (INS_OperandIsMemory(ins, 0) && INS_OperandIsReg(ins, 1)) {
									if (INS_hasKnownMemorySize(ins)) {
											INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)cmpMemReg,
														IARG_ADDRINT, INS_Address(ins),
														IARG_PTR, new string(INS_Disassemble(ins)),
														IARG_MEMORYOP_EA, 0,
														IARG_MEMORYREAD_SIZE,
														IARG_UINT64, INS_OperandReg(ins, 1),
														IARG_CONTEXT, IARG_END);			
									}
									else {
											INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)cmpMemReg,
														IARG_ADDRINT, INS_Address(ins),
														IARG_PTR, new string(INS_Disassemble(ins)),
														IARG_MEMORYOP_EA, 0,
														IARG_UINT32, 16,
														IARG_UINT64, INS_OperandReg(ins, 1),
														IARG_CONTEXT, IARG_END);			

									}
						        }
						else if (INS_OperandIsMemory(ins, 1) && INS_OperandIsReg(ins, 0)) {
									if (INS_hasKnownMemorySize(ins)) {
											INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)cmpMemReg,
														IARG_ADDRINT, INS_Address(ins),
														IARG_PTR, new string(INS_Disassemble(ins)),
														IARG_MEMORYOP_EA, 0,
														IARG_MEMORYREAD_SIZE,
														IARG_UINT64, INS_OperandReg(ins, 0),
														IARG_CONTEXT, IARG_END);			
									}
									else {
											INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)cmpMemReg,
														IARG_ADDRINT, INS_Address(ins),
														IARG_PTR, new string(INS_Disassemble(ins)),
														IARG_MEMORYOP_EA, 0,
														IARG_UINT32, 16,
														IARG_UINT64, INS_OperandReg(ins, 0),
														IARG_CONTEXT, IARG_END);			

									}
						    }
							else {
									cout << "[BUG] Undefined type of cmp statement : " << INS_Disassemble(ins) << " : please report" << endl;
								}
            }    
            if (INS_Disassemble(ins).rfind("call") == 0) {

                INS_InsertCall(
                               ins, IPOINT_BEFORE,(AFUNPTR)taintEngine.callFunction,
                               IARG_ADDRINT, INS_Address(ins),
                               IARG_PTR, new string(INS_Disassemble(ins)),
                               IARG_BRANCH_TARGET_ADDR,
                               IARG_END);
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
    IMG_AddInstrumentFunction(Image, 0);
    PIN_AddSyscallEntryFunction(Syscall_entry, 0);
    TRACE_AddInstrumentFunction(Trace, 0);

    PIN_StartProgram();

    return 0;
}