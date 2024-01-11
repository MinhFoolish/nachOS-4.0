/* Chương trình để test syscall Exec */

#include "syscall.h"
#define stdin 0
#define stdout 1

int main() {
    int pid;
    char *buffer[50];
    Write("Enter a program name (ctrl + D to stop typing): ", 49, stdout);
    Read(buffer, 50, stdin);
    pid = Exec(buffer);
    if (pid < 0) {
        Write("Exec failed: ", 14, stdout);
        PrintNum(pid);
    } else
        Join(pid);
}