#include "syscall.h"
#define BUFFER_SIZE 256
int main() {
    char filename[BUFFER_SIZE];
    int fileId;

    // Nhap ten file
    PrintString("Nhap filename: ");
    ReadString(filename, BUFFER_SIZE);
    // Tao file su dung System call Create
    fileId = Create(filename);

    if (fileId == -1) {
        PrintString("Error: Unable to create the file.\n");
        Halt();
    }
    else {
        PrintString("File created successfully.\n");
    }

    Halt();
}