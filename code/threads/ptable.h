#include "bitmap.h"
#include "pcb.h"
#include "synch.h"
#define MAX_PROCESS 10

class PTable
{
private:
  int psize;
  Bitmap *bm;
  PCB *pcb[MAX_PROCESS];
  Semaphore *bmsem;

public:
  PTable(int size);
  ~PTable();
  int ExecUpdate(char *);
  int ExitUpdate(int);
  int JoinUpdate(int);
  int GetFreeSlot();
  bool IsExist(int pid);
  void Remove(int pid);
  char* GetFileName(int id);
};