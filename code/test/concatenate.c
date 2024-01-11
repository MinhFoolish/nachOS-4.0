#include "syscall.h"
#define MAXLENGTH 256
int main() {
    char filename1[256];
    char filename2[256];
    char s1[256];
    char s2[256];
    int result;
    int ID;
    int pos;
    PrintString("Nhap source filename 1: ");
    ReadString(filename1,MAXLENGTH);

    PrintString("Nhap source filename 2: ");
    ReadString(filename2,MAXLENGTH);
  
    ID = Open(filename1, 1);
    if (ID == -1) {
        PrintString("Error:  khong the mo sourcefile 1.\n");
        Halt();
        return 0;
    }

    Read(s1, MAXLENGTH, ID);
    Close(ID);

    ID = Open(filename2, 1);
    if (ID==-1)
    {
        PrintString("Error: khong the mo sourcefile 2.\n");
        Halt();
        return 0;
    }

    Read(s2,MAXLENGTH,ID);
    Close(ID);
    
    // Tạo file lưu kết quả
    ID = Open("testConcate.txt", 0);
    if (ID != -1) {
        Close(ID);
        Remove("testConcate.txt");
    }
    ID = Create("testConcate.txt");
    Close(ID);
    if (ID == -1) {
        PrintString("Error: khong the mo file ket qua.\n");
        Halt();
        return 0;
    }

    ID = Open("testConcate.txt", 0);
    // Xác định kích thước
    pos = 0;
    while(s1[pos] != '\0') {
        pos++;
    }
    Write(s1, pos, ID);  
    // Xác định kích thước
    pos = 0;
    while(s2[pos] != '\0') {
        pos++;
    }
    Write(s2, pos, ID);

    if (Close(ID) == 0)
    {
        PrintString("\nConcat thanh cong.\n");
        return 1;
    }
    else
    {
        PrintString("Khong the concat.\n");
        return 0;
    }
    
    Halt();
}
