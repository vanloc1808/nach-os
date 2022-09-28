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
	int idx;
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

void ProcessPCRegister() {
	/* set previous programm counter (debugging only)*/
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	
	/* set next programm counter for brach execution */
	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

/**
	input: number at register 4 and 5
	output: sum of them
*/
void SystemCallAdd() {
	DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
	
	/* Process SysAdd Systemcall*/
	int result;
	result = SysAdd(/* int operand1 */(int)kernel->machine->ReadRegister(4),
			/* int operand2 */(int)kernel->machine->ReadRegister(5));

	DEBUG(dbgSys, "Add returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
}

void SystemCallExit() {
	DEBUG(dbgSys, "Exit system call.\n");
	kernel->systemLock->Release();
	kernel->currentThread->Finish();
}

/**
	input: int, the address of thread
	output: -1 if failed, thread id if successful
*/
void SystemCallExec() {
	DEBUG(dbgSys, "Exec system call.\n");

	int virtualAddress = kernel->machine->ReadRegister(4);

	char* name = User2System(virtualAddress, MAX_FILENAME_LEN + 1);

	if (name == NULL) {
		DEBUG(dbgSys, "Not enough memory space.\n");
		printf("Error. Not enough memory space.\n");

		kernel->machine->WriteRegister(2, -1);

		ProcessPCRegister();

		return;
	}

	// OpenFile* of = fileSystem->Open(name);
	// if (of == NULL) {
	// 	DEBUG(dbgSys, "Cannot open this file.\n");
	// 	print("Error. Cannot open this file.\n");

	// 	kernel->machine->WriteRegister(2, -1);

	// 	ProcessPCRegister();

	// 	return;
	// }

	// delete of;

	// int id = pageTable->ExecUpdate(name);
	// kernel->machine->WriteRegister(2, id);

	delete []name;

	return;
}

void ExceptionHandler(ExceptionType which) {
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
	case NoException:
		return;
    case SyscallException:
      switch(type) {
		case SC_Halt:
			// input: no
			// output: halt (shutdown) the system
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
			SysHalt();
			ASSERTNOTREACHED(); // I haven"t understood what this line of code is used for, so I let it lie here
			break;

		case SC_Add:
			SystemCallAdd();			
			/* Modify return point */
			ProcessPCRegister();
			// return; // why they put a return here? .-.			
			ASSERTNOTREACHED();
			break;

		case SC_Exit:
			SystemCallExit();
			ProcessPCRegister();
			ASSERTNOTREACHED();
			break;
		
		case SC_Exec:
			SystemCallExec();
			ProcessPCRegister();
			ASSERTNOTREACHED();
			break;

		default:
		cerr << "Unexpected system call " << type << "\n";
		break;
      }
      break;
    default:
      cerr << "Unexpected user mode exception" << (int)which << "\n";
      break;
    }
	ASSERTNOTREACHED(); 
}
