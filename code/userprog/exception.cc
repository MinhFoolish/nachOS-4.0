	// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
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
#include "filesys.h"
#include "synchconsole.h"

#define MaxFileLength 32
#define INT_MIN (-2147483647 - 1)

void IncreasePC() {
	int counter;
	counter = kernel->machine->ReadRegister(PCReg);
   	kernel->machine->WriteRegister(PrevPCReg, counter);
    counter = kernel->machine->ReadRegister(NextPCReg);
    kernel->machine->WriteRegister(PCReg, counter);
   	kernel->machine->WriteRegister(NextPCReg, counter + 4);
}

char* User2System(int virtAddr,int limit) { 
	int i;// index 
	int oneChar; 
	char* kernelBuf = NULL; 
	kernelBuf = new char[limit +1];//need for terminal string 
	if (kernelBuf == NULL) 
	return kernelBuf; 
	memset(kernelBuf,0,limit+1); 
	//printf("\n Filename u2s:"); 
	for (i = 0 ; i < limit ;i++) { 
		kernel->machine->ReadMem(virtAddr+i,1,&oneChar); 
		kernelBuf[i] = (char)oneChar; 
		//printf("%c",kernelBuf[i]); 
		if (oneChar == 0) 
		break; 
	} 
	return kernelBuf; 
} 

int System2User(int virtAddr, int len ,char* buffer) { 
	if (len < 0) return -1; 
	if (len == 0)return len; 
	int i = 0; 
	int oneChar = 0 ; 
	do{ 
		oneChar= (int) buffer[i]; 
		kernel->machine->WriteMem(virtAddr+i,1,oneChar); 
		i ++; 
	}while(i < len && oneChar != 0); 
	return i; 
}

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
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------


void ExceptionHandler(ExceptionType which) {
    int type = kernel->machine->ReadRegister(2);

    //DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
    case NoException:
        return;

    case SyscallException: {
        switch (type) {
        	case SC_Halt:
				DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
				printf("\n\n Shutdown, initiated by user program.");

				SysHalt();
				ASSERTNOTREACHED();
				break;

			case SC_Add:
				DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
				
				/* Process SysAdd Systemcall*/
				int result;
				result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
						/* int op2 */(int)kernel->machine->ReadRegister(5));

				DEBUG(dbgSys, "Add returning with " << result << "\n");
				/* Prepare Result */
				kernel->machine->WriteRegister(2, (int)result);
				
				/* Modify return point */
				{
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
				
				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
				}

				return;
				ASSERTNOTREACHED();
				break;

			case SC_ReadString:
				int addr_rs;
				addr_rs = kernel->machine->ReadRegister(4);
				int length;
				length = kernel->machine->ReadRegister(5);
				char *buffer_rs;
				buffer_rs = new char[256];
				//buffer_rs = User2System(addr_rs, length);

				int index_rs;
				index_rs = 0;
				char ch;
				while (index_rs < length) {
					ch = kernel->synchConsoleIn->GetChar();
					if (ch == '\0' || ch == '\n') {
						buffer_rs[index_rs] = '\0';
						break;
					}
					buffer_rs[index_rs++] = ch;
				}
				System2User(addr_rs, index_rs + 1, buffer_rs);
    			delete[] buffer_rs;
				IncreasePC();
				break;

			case SC_PrintString:
				int addr_ps;
				addr_ps = kernel->machine->ReadRegister(4); 
				char* buffer_ps;
				buffer_ps = User2System(addr_ps, 255); 

				int index_ps;
				index_ps = 0;
				while (buffer_ps[index_ps] != '\0') {
					kernel->synchConsoleOut->PutChar(buffer_ps[index_ps]);
					index_ps++;
				}
				delete[] buffer_ps; 
				IncreasePC();
				break;

			// Các system call theo yêu cầu
			/*--------------------------------------------------------------*/
			case SC_Create:
				int fileCreateAddr; 
				char* filenameCreate; 
				DEBUG('a',"\n SC_Create call ..."); 
				DEBUG('a',"\n Reading virtual address of filename"); 
				fileCreateAddr = kernel->machine->ReadRegister(4); 
				DEBUG ('a',"\n Reading filename."); 
				filenameCreate = User2System(fileCreateAddr, MaxFileLength + 1); 
				if (filenameCreate == NULL) { 
					printf("\n Not enough memory in system"); 
					DEBUG('a',"\n Not enough memory in system"); 
					kernel->machine->WriteRegister(2,-1); 
					delete filenameCreate; 
					return; 
				} 
				DEBUG('a',"\n Finish reading filename."); 	
				if (!kernel->fileSystem->Create(filenameCreate, 0)) { 
					printf("\n Error create file '%s'",filenameCreate); 
					kernel->machine->WriteRegister(2, -1); 
					delete filenameCreate; 
					return; 
				}
				
				kernel->machine->WriteRegister(2, 0);
				delete filenameCreate; 
				IncreasePC();
				break;

			case SC_Open:
				int addr;
				addr = kernel->machine->ReadRegister(4);
				int type;
				type = kernel->machine->ReadRegister(5);
				char* filenameOpen;
				filenameOpen = User2System(addr, MaxFileLength);
				int slot;
				slot = kernel->fileSystem->CheckSlot();
				if (slot != -1) {
					if (type == 0 || type == 1) { // trường hợp 0: read and write và 1: read only
						kernel->fileSystem->assignFiledescriptorToOf(kernel->fileSystem->Open(filenameOpen, type), slot);
						kernel->machine->WriteRegister(2, slot);
						IncreasePC();
						break; 
					}
					else if (type == 2) { // console input
						kernel->machine->WriteRegister(2, 0);
						IncreasePC();
						break;
					}
					else { // console output
						kernel->machine->WriteRegister(2, 1);
						IncreasePC();
						break;
					}
					break;
				}
				kernel->machine->WriteRegister(2, -1); 
				delete[] filenameOpen;
				IncreasePC();
				break;

			case SC_Close:
				int fileID;
				fileID = kernel->machine->ReadRegister(4);
				if (fileID >=0 && fileID <= MAX_FILE - 1) {
					if (kernel->fileSystem->of[fileID]) {
						delete kernel->fileSystem->of[fileID];
						kernel->fileSystem->of[fileID] = NULL;
						kernel->machine->WriteRegister(2, 0);
						IncreasePC();
						break;
					}
				}
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				break;	

			case SC_Read:
				int addrRead;
				addrRead = kernel->machine->ReadRegister(4);
				int lenRead;
				lenRead = kernel->machine->ReadRegister(5);
				int idRead;
				idRead = kernel->machine->ReadRegister(6);
				int oPosRead;
				int nPosRead;
				char* bufRead;
				
				if (idRead < 2 || idRead > MAX_FILE - 1) { // File không nằm trong khoảng 20 file discriptor
					kernel->machine->WriteRegister(2, -1);
					IncreasePC();
					break;
				}
				
				if (kernel->fileSystem->of[idRead] == NULL) { // File không tồn tại
					kernel->machine->WriteRegister(2, -1); 
					IncreasePC();
					break;
				}

				if (kernel->fileSystem->of[idRead]->type == 3) { //Console OUT
					kernel->machine->WriteRegister(2, -1);
					IncreasePC();
					break;
				}

				oPosRead = kernel->fileSystem->of[idRead]->GetCurrentPos();
				bufRead = User2System(addrRead, lenRead); 

				if (kernel->fileSystem->of[idRead]->type == 2) { //Console IN
					char chRead;
					int sizeRead;
					sizeRead = 0;
					while (sizeRead < lenRead) {
						chRead = kernel->synchConsoleIn->GetChar();
						if (chRead == '\0' || chRead == '\n' || chRead == EOF ) {
							break;
						}
						bufRead[sizeRead] = chRead;
						sizeRead++;
					}

					if (sizeRead > 0) {
						System2User(addrRead, sizeRead, bufRead); 
						kernel->machine->WriteRegister(2, sizeRead); 
					}
					else {
						kernel->machine->WriteRegister(2, -1);
					}
					
					delete bufRead;
					IncreasePC();
					break;
				}
				if ((kernel->fileSystem->of[idRead]->Read(bufRead, lenRead)) > 0) {
					nPosRead = kernel->fileSystem->of[idRead]->GetCurrentPos();
					int sizeReal;
					sizeReal = nPosRead - oPosRead;
					System2User(addrRead, sizeReal, bufRead); 
					kernel->machine->WriteRegister(2, sizeReal);
					DEBUG(dbgSys, sizeReal);
				}
				else {
					kernel->machine->WriteRegister(2, -1);
				}
				delete bufRead;
				IncreasePC();
				break;

			case SC_Write:
				int addrWrite;
				addrWrite = kernel->machine->ReadRegister(4); 
				int lenWrite;
				lenWrite = kernel->machine->ReadRegister(5); 
				int idWrite;
				idWrite = kernel->machine->ReadRegister(6); 
				int oPosWrite;
				int nPosWrite;
				int results;
				char *bufWrite;
				DEBUG(dbgSys, "Come here : " << 0);
				if (idWrite < 2 || idWrite > MAX_FILE - 1) { 
					kernel->machine->WriteRegister(2, -1);
					IncreasePC();
					break;
				}

				if (kernel->fileSystem->of[idWrite] == NULL) {
					kernel->machine->WriteRegister(2, -1);
					IncreasePC();
					break;
				}

				// Nếu type file với trường hợp 1: read only và 2: console input
				if (kernel->fileSystem->of[idWrite]->type == 1 || kernel->fileSystem->of[idWrite]->type == 2) {
					kernel->machine->WriteRegister(2, -1);
					IncreasePC();
					break;
				}
				DEBUG(dbgSys, "Come here : " << 1);
				oPosWrite = kernel->fileSystem->of[idWrite]->GetCurrentPos(); 
				bufWrite = User2System(addrWrite, lenWrite);  
				
				if (kernel->fileSystem->of[idWrite]->type == 0) { // Xử lý trường hợp Read and write
					DEBUG(dbgSys, "Come here : " << 2);
					results = kernel->fileSystem->of[idWrite]->Write(bufWrite, lenWrite);
					if (results > 0)
					{
						nPosWrite = kernel->fileSystem->of[idWrite]->GetCurrentPos();
						DEBUG(dbgSys, "Result of writting file: " << results);
						kernel->machine->WriteRegister(2, results);
						delete[] bufWrite;
						IncreasePC();
						break;
					}
				}

				if (kernel->fileSystem->of[idWrite]->type == 3) { // Xử lý trường hợp console output
					int i = 0;
					while (bufWrite[i] != 0 && bufWrite[i] != '\n') {
						kernel->synchConsoleOut->PutChar(bufWrite[i]);
						i++;
					}
					bufWrite[i] = '\n';
					kernel->synchConsoleOut->PutChar(bufWrite[i]); 
					kernel->machine->WriteRegister(2, i - 1); 
					delete bufWrite;
					IncreasePC();
					break;
				}

			case SC_Seek:
				int posSeek;
				posSeek = kernel->machine->ReadRegister(4); 
				int idSeek;
				idSeek = kernel->machine->ReadRegister(5); 
				if (idSeek < 0 || idSeek > MAX_FILE) {
					kernel->machine->WriteRegister(2, -1);
					IncreasePC();
					break;
				}

				if (kernel->fileSystem->of[idSeek] == NULL) {
					kernel->machine->WriteRegister(2, -1);
					IncreasePC();
					break;
				}

				if (idSeek == 0 || idSeek == 1) { // Kiem tra co goi Seek tren console khong
					kernel->machine->WriteRegister(2, -1);
					IncreasePC();
					break;
				}

				posSeek = (posSeek == -1) ? kernel->fileSystem->of[idSeek]->Length() : posSeek;
				if (posSeek > kernel->fileSystem->of[idSeek]->Length() || posSeek < 0) {
					kernel->machine->WriteRegister(2, -1);
				}
				else {
					kernel->fileSystem->of[idSeek]->Seek(posSeek);
					kernel->machine->WriteRegister(2, posSeek);
				}

				IncreasePC();
				break;

			case SC_Remove:
				int addrRemove;
				addrRemove = kernel->machine->ReadRegister(4);
				char *filenameRemove;
				filenameRemove = User2System(addrRemove, MaxFileLength + 1);
				if (filenameRemove == NULL) {
					kernel->machine->WriteRegister(2, -1);
					IncreasePC();
					break;
				}

				if (kernel->fileSystem->Remove(filenameRemove)) {
					DEBUG(dbgSys, "Delete successfully");
					kernel->machine->WriteRegister(2, 0);
				}
				else {
					DEBUG(dbgSys, "Delete failed");
					kernel->machine->WriteRegister(2, -1);
				}	
				
				delete []filenameRemove;
				IncreasePC();
				break;
			/*------------------------------------------------------------*/
			
			/*Socket*/
			case SC_SocketTcp:
               kernel->machine->WriteRegister(2, SysSocketTcp());
               break;

        	case SC_ConnectSocket:
                int connectID;
				connectID = kernel->machine->ReadRegister(4);
				int connectAddr;
				connectAddr = kernel->machine->ReadRegister(5);
				char *ipAddr;
				ipAddr = User2System(connectAddr,255);
				int portID;
				portID = kernel->machine->ReadRegister(6);
				kernel->machine->WriteRegister(2, SysConnectSocket(connectID, ipAddr, portID));
                break;

        	case SC_Send:
                int sendID;
				sendID = kernel->machine->ReadRegister(4);
				int sendAddr;
				sendAddr = kernel->machine->ReadRegister(5);
				char* buffer_send;
				buffer_send = User2System(sendAddr,255);
				int len;
				len = kernel->machine->ReadRegister(6);
				kernel->machine->WriteRegister(2, SysSendSocket(sendID, buffer_send, len));
                break;

        	case SC_Receive:
                int receiveID;
				receiveID = kernel->machine->ReadRegister(4);
				int receiveAddr;
				receiveAddr = kernel->machine->ReadRegister(5);
				char* buffer_re;
				buffer_re = User2System(receiveAddr,255);
				kernel->machine->WriteRegister(2, SysReceiveSocket(receiveID, buffer_re));
				System2User(receiveAddr, 255, buffer_re);
                break;

        	case SC_CloseSocket:
                int closeID;
				closeID = kernel->machine->ReadRegister(4);
    			kernel->machine->WriteRegister(2, SysCloseSocket(closeID));
                break;


			default:
				//cerr << "Unexpected system call " << type << "\n";
				break;
       	}
      	break;

    default:
      //cerr << "Unexpected user mode exception" << (int)which << "\n";
      break;
    }
    ASSERTNOTREACHED();
}
}