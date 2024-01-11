#include "syscall.h"

#define BUFFER_SIZE 256
int main() {
	char filename[BUFFER_SIZE]; 
	char buffer[BUFFER_SIZE];
	int bytesRead,id,len; 

	PrintString("Nhap filename: ");
	ReadString(filename, BUFFER_SIZE);
		
	
	id = Open(filename, 1);

	if (id < 0) {
		PrintString("Error: Khong the mo file.\n");
		Halt();
	}
	len = Seek(id, -1);
	Seek(id, 0);

	if ((bytesRead = Read(buffer, len, id)) > 0) {
		PrintString("Noi dung file: \n");
		PrintString(buffer);
	}

	Close(id);
	Halt();
}