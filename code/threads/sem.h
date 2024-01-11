#include <stdio.h>
#include <string.h>

class Sem
{
private:
  char name[50];  // Semaphore name.
  Semaphore *sem; // Create new Semaphore.

public:
  Sem(char *na, int i)
  {
    strcpy(this->name, na);
    sem = new Semaphore(this->name, i);
  }
  ~Sem()
  {
    if (sem)
      delete sem;
  }

  void wait(){
    sem->P();
  }

  void signal() {
    sem->V();
  }

  char *GetName() {
    return this->name;
  }
};