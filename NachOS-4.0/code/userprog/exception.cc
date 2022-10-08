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
		kernel->interrupt->setStatus(SystemMode);
		DEBUG(dbgSys, "Switch to system mode\n");
		return;
   	case SyscallException:
    		switch(type) {
			case SC_Exit: {
				ProcessPCRegister();
			}
			case SC_Exec: {
				ProcessPCRegister();
			}
			case SC_Join: {
				ProcessPCRegister();
			}
			case SC_Create: {
				ProcessPCRegister();
			}
			case SC_Remove: {
				ProcessPCRegister();
			}
			case SC_Open: {
				ProcessPCRegister();
			}
			case SC_Read: {
				ProcessPCRegister();
			}
			case SC_Write: {
				ProcessPCRegister();
			}
			case SC_Seek: {
				ProcessPCRegister();
			}
			case SC_Close: {
				ProcessPCRegister();
			}
			case SC_ThreadFork: {
				ProcessPCRegister();
			}
			case SC_ThreadYield: {
				ProcessPCRegister();
			}
			case SC_ExecV: {
				ProcessPCRegister();
			}
			case SC_ThreadExit: {
				ProcessPCRegister();
			}
			case SC_ThreadJoin: {
				ProcessPCRegister();
			}
			case SC_Halt: {
				// input: no
				// output: halt (shutdown) the system
				DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
				SysHalt();
				ASSERTNOTREACHED(); 
				break;
			}
			case SC_Add: {
				SystemCallAdd();			
				/* Modify return point */
				ProcessPCRegister();
				return; 
				//ASSERTNOTREACHED();
				break;
			}


		// ---------------------------------------

			case SC_ReadChar: {
				SystemCallReadChar();
				ProcessPCRegister();
				return;
				//ASSERTNOTREACHED();
				break;
			}

			case SC_PrintChar: {
				SystemCallPrintChar();
				ProcessPCRegister();
				return;
				//ASSERTNOTREACHED();

				break;
			}

			case SC_ReadString: {
				SystemCallReadString();
				ProcessPCRegister();
				
				return;
				break;
			}

			case SC_PrintString: {
				SystemCallPrintString();
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
