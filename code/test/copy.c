#include "syscall.h"
#define BUFFER_SIZE 256

int main() {
    char srcFileName[BUFFER_SIZE];
    char destFileName[BUFFER_SIZE];
    int srcFile;
    int destFile;
    char buffer[BUFFER_SIZE];
    int bytesRead;
    int bytesWritten;
    int index;
	char c; //Ky tu de in ra
    PrintString("Nhap source file name: ");
    ReadString(srcFileName, BUFFER_SIZE);

    PrintString("Nhap destination file name: ");
    ReadString(destFileName, BUFFER_SIZE);

    srcFile = Open(srcFileName, 1);
    
    if (srcFile == -1) {
        PrintString("Error: Unable to open source file.\n");
        Halt();
    }

    destFile = Create(destFileName);
 
    if (destFile == -1) {
        PrintString("Error: Unable to create destination file.\n");
        Halt();
    } 

    destFile = Open(destFileName, 0); 

    if (destFile == -1) {
        PrintString("Create dest file failed.\n");
        Halt();
    }
    if ((bytesRead = Read(buffer, BUFFER_SIZE, srcFile)) > 0) {
        bytesWritten = Write(buffer, bytesRead, destFile);
        
        if (bytesWritten != bytesRead || bytesWritten == -1) {
            PrintString("Error: Failed to copy the file.\n");
            Halt();
        } 
    }

    Close(srcFile); 

    PrintString("File copy successful.\n");
    Halt();
}


