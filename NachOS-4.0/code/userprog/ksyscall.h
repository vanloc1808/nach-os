/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"
#include "synchconsole.h"

#define MAX_SIZE 1024


void SysHalt()
{
  kernel->interrupt->Halt();
}


int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

// --------------------------------------------------------------------

// threads/kernel.cc
// userprog/synchconsole.cc
// synchConsoleIn is pointer of SynchConsoleInput
// Having method GetChar()
char SysReadChar()
{
	SynchConsoleInput* inp = (SynchConsoleInput*)kernel->synchConsoleIn;
	return inp->GetChar();
}


void SysPrintChar(char c)
{
	SynchConsoleOutput* out = (SynchConsoleOutput*)kernel->synchConsoleOut;
	out->PutChar(c);
}

void SysReadString(char buffer[], int length)
{
	for (int i = 0; i < length; i++) {
		buffer[i] = SysReadChar();
		if (buffer[i] == '\x00' || buffer[i] == '\n') {
			break;
		}
	}
}

void SysPrintString(char buffer[])
{
	char* i = buffer;
	while (*i) {
		SysPrintChar(*i);
		i++;
	}
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
	//DEBUG(dbgSys, "Exit system call.\n");
	//kernel->systemLock->Release();
	//kernel->currentThread->Finish();
}


// --------------------------------------------------------------------

#endif /* ! __USERPROG_KSYSCALL_H__ */
