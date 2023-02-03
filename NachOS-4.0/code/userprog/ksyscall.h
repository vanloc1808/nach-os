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

#define INT_MAX 2147483647LL
#define INT_MIN (-INT_MAX - 1)
char const* overflow_error = "Overflow\n";
char const* invalid_error = "Invalid\n";


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
			buffer[i] = 0;
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

	if (len == 0) {
		SysPrintString((char*) invalid_error);
		return INT_MIN;
	}

	bool negative = false;
	int start = 0;

	if(buffer[start] == '-'){
		negative = true;
		++start;
	}

	for (int i = start; i < len; ++i){
		if (buffer[i] < '0' || buffer[i] > '9'){
			SysPrintString((char*) invalid_error);
			//SysHalt();
			return 0;								//invalid input
		}
		else {
			// overflow --> return 0:
			//overflow limit: >= 2147483648 = 2^31 or <= -2147483649 = -2^31-1
			if (number > MAX_INT_DIV_10 || (number == MAX_INT_DIV_10 && buffer[i] > '7')){
				if (number == MAX_INT_DIV_10 && buffer[i] == '8' && negative){
					return INT_MIN;
				}
				else{
					SysPrintString((char*) overflow_error);
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

	else if (number == INT_MIN){	//-2^31
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
	return rand();
}

void SysCreate(char* name) {
	if ((name == NULL) || strlen(name) == 0) {
		kernel->machine->WriteRegister(2, -1);
		return;
	} 

	if (kernel->fileSystem->Create(name, 0)) {
		kernel->machine->WriteRegister(2, 0);
	} else {
		kernel->machine->WriteRegister(2, -1);
	}
	return;
} 

void SysRemove(char* name) {
	if ((name == NULL) || strlen(name) == 0) {
		kernel->machine->WriteRegister(2, -1);
		return;
	}

	if (kernel->fileSystem->Remove(name) == 1) {
		kernel->machine->WriteRegister(2, 0);
	} else {
		kernel->machine->WriteRegister(2, -1);
	}
	return;
}


int SysRead(char* buffer, int length, int fd) {
	// OpenFile* of = new OpenFile(fd);
	OpenFile* of = kernel->fileSystem->get(fd);
	if (of == NULL) return -1;
	if (!of->isReadable()) {
		return -1;
	}
	int value = of->Read(buffer, length);
	return value;
}

int SysWrite(char* buffer, int length, int fd) {
	OpenFile* of = kernel->fileSystem->get(fd);
	if (of == NULL) return -1;
	if (of == NULL) return -1;
	if (!of->isWritable()) {
		return -1;
	}
	int value = of->Write(buffer, length);
	return value;
}

int SysSeek(int position, int fd) {
	OpenFile* of = kernel->fileSystem->get(fd);
	if (of == NULL) return -1;
	int value = of->Seek(position);
	return value;
}

int SysOpen(char* fileName, int accessType) {
	return kernel->fileSystem->Open(fileName, accessType);
}

int SysClose(int fd) {
	return kernel->fileSystem->Close(fd);
}

// --------------------------------------------------------------------

/**
	function to copy memory from user space to kernel space
	@param virtualAddress: address of memory
	@param limit: limit of buffer
	@param keepNULL: keep null byte or not (for string)
	@return the recorded buffer
*/
char* User2System(int virtualAddress, int limit, int keepNULL = 0) {
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

		if (oneChar == 0 && !keepNULL) { // Accept null bytes to be read
			break;
		}
	}

	return kernelBuffer;
}

/* 
 * Input: - User space address (int) 
 *  - Limit of buffer (int) 
 *   - Buffer (char[]) 
 * 	- keepNULL: keep null byte or not (for string)
 *   Output:- Number of bytes copied (int) 
 *   Purpose: Copy buffer from System memory space to User memory space 
 *   */ 
int System2User(int virtAddr,int len,char* buffer, int keepNULL = 0) 
{ 
	if (len < 0) return -1; 
	if (len == 0)return len; 
	int i = 0; 
	int oneChar = 0 ; 
	do { 
		oneChar= (int) buffer[i]; 
		kernel->machine->WriteMem(virtAddr+i,1,oneChar); 
		i++; 
	} while(i < len && (oneChar != 0 || keepNULL));  // accept NULL bytes to be written!
	return i; 
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

void SystemCallReadChar() {
	int tmp;
	DEBUG(dbgSys, "\nReading a character from console!");
	tmp = (int)SysReadChar(); // Convert char to 32 bit int
	DEBUG(dbgSys, "\nRECEIVE: " << char(tmp) << "\n");
	// Save result to r2
	kernel->machine->WriteRegister(2, tmp);
}

void SystemCallPrintChar() {
	int tmp;
	DEBUG(dbgSys, "\nPrinting a character to console!");
	tmp = kernel->machine->ReadRegister(4);
	DEBUG(dbgSys, "\nSEND: " << tmp << "\n");
	SysPrintChar(char(tmp)); 
}

void SystemCallReadString() {
	DEBUG(dbgSys, "\nReading a string from console!");
	int virtualAddr;
	int length;
	virtualAddr = kernel->machine->ReadRegister(4);
	length = kernel->machine->ReadRegister(5);

	// Handle special case!
	if (length < 0 || length >= MAX_SIZE) {
		SysPrintString((char*) invalid_error);
		DEBUG(dbgSys, "\nNegative or too large size!");
		SysHalt();
		return;
	}

	char* buffer = new char[length];
	SysReadString(buffer, length);
	DEBUG(dbgSys, "\nSystem received: " << buffer);
	DEBUG(dbgSys, "\nTransmitting to User space!");

	System2User(virtualAddr, length, buffer);
	delete[] buffer;
}

void SystemCallPrintString() {
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
}

void SystemCallCreate() {
	int bufferWrite;
	char* tempWrite;
	bufferWrite = kernel->machine->ReadRegister(4);

	tempWrite = User2System(bufferWrite, MAX_SIZE);
	
	SysCreate(tempWrite);
	delete[] tempWrite;
}

void SystemCallRemove() {
	int bufferWrite;
	char* tempWrite;
	bufferWrite = kernel->machine->ReadRegister(4);

	tempWrite = User2System(bufferWrite, MAX_SIZE);

	SysRemove(tempWrite);
	delete[] tempWrite;
}

void SystemCallRead() {
	DEBUG(dbgSys, "\n Reading file");
	int virtAddr = kernel->machine->ReadRegister(4);
	int length = kernel->machine->ReadRegister(5);
	int fd = kernel->machine->ReadRegister(6);

	char* buffer = new char[length];
	if (buffer == NULL || fd == 1 || fd < 0) {
		SysPrintString((char*) invalid_error); // Todo: better error comment
		kernel->machine->WriteRegister(2, -1);
		return;
	} else if (fd == 0) { // Handle console
		SynchConsoleInput* inp = (SynchConsoleInput*)kernel->synchConsoleIn;
		int i = 0;
		while (i < length) {
			buffer[i] = inp->GetChar();
			if (buffer[i] == EOF) { // End of read
				buffer[i] = 0;
				break;
			}
			i++;
		}
		System2User(virtAddr, i, buffer, 1);
		kernel->machine->WriteRegister(2, i);
	} else {
		int result = SysRead(buffer, length, fd);
		// DEBUG(dbgSys, "GET: " << result << "\n");
		// System to User space
		if (result != -1)
			System2User(virtAddr, result, buffer);

		kernel->machine->WriteRegister(2, result);
	}
	delete[] buffer;
}

void SystemCallWrite() {
	DEBUG(dbgSys, "\n Writing file");
	int virtAddr = kernel->machine->ReadRegister(4);
	int length = kernel->machine->ReadRegister(5);
	int fd = kernel->machine->ReadRegister(6);
	
	// User to System space
	char* buffer = User2System(virtAddr, length, 1);

	if (buffer == NULL || fd <= 0) {
		SysPrintString((char*) invalid_error);
		kernel->machine->WriteRegister(2, -1);
		return;
	} else if (fd == 1) { // Handle console
		SynchConsoleOutput* out = (SynchConsoleOutput*)kernel->synchConsoleOut;
		for (int i = 0; i < length; i++) {
			out->PutChar(buffer[i]);
		}
		kernel->machine->WriteRegister(2, length); // Todo: suppose writing to console alway perfect!
	} else {
		int result = SysWrite(buffer, length, fd);
		kernel->machine->WriteRegister(2, result);
	}
	delete[] buffer;
}

void SystemCallSeek() {
	DEBUG(dbgSys, "\n Seek file");
	int position = kernel->machine->ReadRegister(4);
	int fd = kernel->machine->ReadRegister(5);

	// Handle fd 0|1
	if (fd < 2) {
		SysPrintString((char*) invalid_error);
		kernel->machine->WriteRegister(2, -1);
		return;
	}

	int result = SysSeek(position, fd);
	kernel->machine->WriteRegister(2, result);
}

void SystemCallOpen() {
	DEBUG(dbgSys, "\n Open file");
	int virtAddr = kernel->machine->ReadRegister(4);
	int accessType = kernel->machine->ReadRegister(5);

	char* buffer = User2System(virtAddr, MAX_SIZE);

	int result = SysOpen(buffer, accessType);

	kernel->machine->WriteRegister(2, result);

	delete[] buffer;
}

void SystemCallClose() {
	DEBUG(dbgSys, "\n Close file");
	int fd = kernel->machine->ReadRegister(4);
	int result = SysClose(fd);
	kernel->machine->WriteRegister(2, result);
}

// --------------------------------------------------------------------

#endif /* ! __USERPROG_KSYSCALL_H__ */
