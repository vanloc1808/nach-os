// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can"t handle.
//	For instance, accessing memory that doesn"t exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don"t forget to increment the pc
// before returning. (Or else you"ll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------

/**
	function to copy memory from user space to kernel space
	@param virtualAddress: address of memory
	@param limit: limit of buffer
	@return the recorded buffer
*/
char* User2System(int virtualAddress, int limit) {
	// int idx;
	int oneChar;
	char* kernelBuffer = NULL;

	kernelBuffer = new char[limit + 1]; // space for "\0"

	if (kernelBuffer == NULL) {
		return kernelBuffer;
	}

	memset(kernelBuffer, 0, limit + 1);

	for (int i = 0; i < limit; i++) {
		kernel->machine->ReadMem(virtualAddress + i, 1, &oneChar);
		kernelBuffer[i] = (char)oneChar;

		if (oneChar == 0) {
			break;
		}
	}

	return kernelBuffer;
}

/* 
 * Input: - User space address (int) 
 *  - Limit of buffer (int) 
 *   - Buffer (char[]) 
 *   Output:- Number of bytes copied (int) 
 *   Purpose: Copy buffer from System memory space to User memory space 
 *   */ 
int System2User(int virtAddr,int len,char* buffer) 
{ 
	if (len < 0) return -1; 
	if (len == 0)return len; 
	int i = 0; 
	int oneChar = 0 ; 
	do { 
		oneChar= (int) buffer[i]; 
		kernel->machine->WriteMem(virtAddr+i,1,oneChar); 
		i++; 
	} while(i < len && oneChar != 0); 
	return i; 
} 



void ProcessPCRegister() {
	/* set previous programm counter (debugging only)*/
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	
	/* set next programm counter for brach execution */
	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

void ExceptionHandler(ExceptionType which) {
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
	case NoException:
		return;
   	case SyscallException:
    		switch(type) {
		case SC_Halt: {
			// input: no
			// output: halt (shutdown) the system
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
			SysHalt();
			//ASSERTNOTREACHED(); // I haven"t understood what this line of code is used for, so I let it lie here
			break;
		}
		case SC_Add: {
			SystemCallAdd();			
			/* Modify return point */
			ProcessPCRegister();
			return; // why they put a return here? .-.			
			//ASSERTNOTREACHED();
			break;
		}
		case SC_Exit: {
			SystemCallExit();
			ProcessPCRegister();
			//ASSERTNOTREACHED();
			break;
		}


	// ---------------------------------------

		case SC_ReadChar: {
			int tmp;
			DEBUG(dbgSys, "\nReading a character from console!");
			tmp = (int)SysReadChar(); // Convert char to 32 bit int
			DEBUG(dbgSys, "\nRECEIVE: " << char(tmp) << "\n");
			// Save result to r2
			kernel->machine->WriteRegister(2, tmp);
			ProcessPCRegister();
			return;
			//ASSERTNOTREACHED();
			break;
		}

		case SC_PrintChar: {
			int tmp;
			DEBUG(dbgSys, "\nPrinting a character to console!");
			tmp = kernel->machine->ReadRegister(4);
			DEBUG(dbgSys, "\nSEND: " << tmp << "\n");
			SysPrintChar(char(tmp)); 
			ProcessPCRegister();
			return;
			//ASSERTNOTREACHED();

			break;
		}

		case SC_ReadString: {
			DEBUG(dbgSys, "\nReading a string from console!");
			int virtualAddr;
			int length;
			virtualAddr = kernel->machine->ReadRegister(4);
			length = kernel->machine->ReadRegister(5);

			// Handle special case!
			if (length < 0 || length >= MAX_SIZE) {
				// kernel->machine->RaiseException(NumExceptionTypes, virtualAddr);
				DEBUG(dbgSys, "\nNegative or too large size!");
				return;
			}

			char* buffer = new char[length];
			SysReadString(buffer, length);
			DEBUG(dbgSys, "\nSystem received: " << buffer);
			DEBUG(dbgSys, "\nTransmitting to User space!");

			System2User(virtualAddr, length, buffer);
			delete[] buffer;
			ProcessPCRegister();
			
			return;
			break;
		}

		case SC_PrintString: {
			DEBUG(dbgSys, "\nPrinting a string to console!");
			int virtualAddr;
			virtualAddr = kernel->machine->ReadRegister(4);
			char* buffer;
			bool flag = true;
			while(flag) {
				buffer = User2System(virtualAddr, MAX_SIZE);
				SysPrintString(buffer);
				// Check end of string!
				if (buffer[MAX_SIZE - 1] == 0) {
					DEBUG(dbgSys, "\nEnd!\n");
					flag = false;
				}
				virtualAddr += MAX_SIZE;
				delete[] buffer;
			}
			ProcessPCRegister();
			
			return;
			break;
		}

		case SC_ReadNum:
		{
			SystemCallReadNum();			
			ProcessPCRegister();
			return;
			//ASSERTNOTREACHED();
			break;
		}

		case SC_PrintNum:
		{
			SystemCallPrintNum();
			ProcessPCRegister();
			return;
			//ASSERTNOTREACHED();
			break;
		}

		case SC_RandomNum:
		{
			SystemCallRandomNum();
			ProcessPCRegister();
			return;
			//ASSERTNOTREACHED();
			break;
		}

		// ---------------------------------------
		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
      		}
      		break;
	case PageFaultException:
		DEBUG(dbgSys, "No valid translation found\n");
		printf("No valid translation found\n");
		kernel->interrupt->Halt();
		//ASSERTNOTREACHED();
		break;
	case ReadOnlyException:
		DEBUG(dbgSys, "Write attempted to page marked \"read-only\"\n");
		printf("Write attempted to page marked \"read-only\"\n");
		kernel->interrupt->Halt();
		//ASSERTNOTREACHED();
		break;
	case BusErrorException:
		DEBUG(dbgSys, "Translation resulted in an invalid physical address\n");
		printf("Translation resulted in an invalid physical address\n");
		kernel->interrupt->Halt();
		//ASSERTNOTREACHED();
		break;
	case AddressErrorException:
		DEBUG(dbgSys, "Unaligned reference or one that was beyond the end of the address space\n");
		printf("Unaligned reference or one that was beyond the end of the address space\n");
		kernel->interrupt->Halt();
		//ASSERTNOTREACHED();
		break;
	case OverflowException:
		DEBUG(dbgSys, "Integer overflow in add or sub.\n");
		printf("Integer overflow in add or sub.\n");
		kernel->interrupt->Halt();
		//ASSERTNOTREACHED();
		break;
	case IllegalInstrException:
		DEBUG(dbgSys, "Unimplemented or reserved instr.\n");
		printf("Unimplemented or reserved instr.\n");
		kernel->interrupt->Halt();
		//ASSERTNOTREACHED();
		break;
	case NumExceptionTypes:
		DEBUG(dbgSys, "NumExceptionTypes\n");
		printf("NumExceptionTypes\n");
		kernel->interrupt->Halt();
		//ASSERTNOTREACHED();
		break;
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
    }
	ASSERTNOTREACHED(); 
}
