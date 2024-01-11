#include "synch.h"

class PCB
{
private:
    Semaphore *joinsem;
    Semaphore *exitsem;
    Semaphore *multex;
    int exitcode;
    int numwait;

    Thread *thread;
    char filename[128];

public:
    int processID;
    int parentID;
    PCB(int id);
    ~PCB();
    int Exec(char *filename, int pid);
    int GetID();
    int GetNumWait();
    void JoinWait();
    void ExitWait();
    void JoinRelease();
    void ExitRelease();
    void IncNumWait();
    void DecNumWait();
    void SetExitCode(int ec);
    int GetExitCode();

    void SetFileName(char *fn);
    char *GetFileName();
};
