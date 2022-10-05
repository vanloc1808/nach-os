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

//System call read an integer ([-2^32, 2^32 - 1]) in decimal form
//Return 0 if:
//+ invalid input. Ex: b44a5	(we do not handle hexadecimal base)
int SysReadNum()
{
	int number = 0;
	int max_len = 11;					//INT_MIN = -2147483648; INT_MAX = 2147483647
	int MAX_INT_DIV_10 = 214748364;
	char buffer[max_len + 1];
	SysReadString(buffer, max_len + 1);
	char *c = buffer;
	int len = 0;
	
	while (*c != '\x00' && *c != '\n'){
		++len;
		++c;
	}

	bool negative = false;
	int start = 0;

	if(buffer[start] == '-'){
		negative = true;
		++start;
	}

	for (int i = start; i < len; ++i){
		if (buffer[i] < '0' || buffer[i] > '9'){
			SysPrintString("Invalid input...\n");
			//SysHalt();
			return 0;								//invalid input
		}
		else {
			// overflow --> return 0:
			//overflow limit: >= 2147483648 = 2^31 or <= -2147483649 = -2^31-1
			if (number > MAX_INT_DIV_10 || (number == MAX_INT_DIV_10 && buffer[i] > '7')){
				if (number == MAX_INT_DIV_10 && buffer[i] == '8' && negative){
					return -2147483648;
				}
				else{
					SysPrintString("Overflow...\n");
					//SysHalt();
					return 0;			//overflow
				}
			}
			else {
				number = number * 10 + (buffer[i] - '0');
			}
		}
	}

	if (negative){
		number = -number;
	}

	return number;
}

void SysPrintNum(int number)
{
	int len = 0;
	int start = 0;
	int tmp = number;
	char numToStr[12];
	for (int i = 0; i < 12; ++i){
		numToStr[i] = 0x00;
	}

	if (number == 0){
		len = 1;
		numToStr[0] = '0';
	}

	else if (number == -2147483648){	//-2^31
		len = 11;
		numToStr[0] = '-';
		numToStr[1] = '2';
		numToStr[2] = '1';
		numToStr[3] = '4';
		numToStr[4] = '7';
		numToStr[5] = '4';
		numToStr[6] = '8';
		numToStr[7] = '3';
		numToStr[8] = '6';
		numToStr[9] = '4';
		numToStr[10] = '8';
	}
	else {
		if (number < 0){
			len = 1;
			number = -number;
			numToStr[0] = '-';
			start = 1;
		}
		tmp = number;
		while(tmp){
			tmp = tmp/10;
			++len;
		}
		tmp = number;
		for (int i = len - 1; i >= start; --i){
			numToStr[i] = (char)(tmp % 10 + '0');
			tmp = tmp/10;
		}
	}

	SysPrintString(numToStr);

}

int SysRandomNum()
{
	return Random();
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

void SystemCallReadNum(){
	DEBUG(dbgSys, "Reading an integer from console...\n");

	int result;
	result = SysReadNum();

	DEBUG(dbgSys, "Returned value is " << result << "\n");
	kernel->machine->WriteRegister(2, result);
}


void SystemCallPrintNum(){
	DEBUG(dbgSys, "Print" << kernel->machine->ReadRegister(4) << "\n");

	SysPrintNum(kernel->machine->ReadRegister(4));

	DEBUG(dbgSys, "Printed successful!\n");
}

void SystemCallRandomNum(){
	DEBUG(dbgSys, "Generate a random number...\n");
	int result;
	result = SysRandomNum();

	DEBUG(dbgSys, "Returned value is " << result << "\n");
	kernel->machine->WriteRegister(2, result);
}

// --------------------------------------------------------------------

#endif /* ! __USERPROG_KSYSCALL_H__ */
