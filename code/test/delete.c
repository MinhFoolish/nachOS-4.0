#include "syscall.h"
#define MAXLENGTH 256
int main() {
    int result;
    char filename[MAXLENGTH];
    PrintString("Nhap filename: ");
    ReadString(filename, MAXLENGTH);
    result = Remove(filename);
    
    if (result == -1) {
        PrintString("Khong the xoa file.\n");
    }
    if (result == 0) {
        PrintString("Xoa thanh cong.\n");
    }
    Halt();
}