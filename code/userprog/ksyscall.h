/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"

void SysHalt()
{
    kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
    return op1 + op2;
}

int SysSocketTcp()
{
    return kernel->socket->Insert();
}

int SysConnectSocket(int socketId, char *ipAddress, int portID)
{
    return kernel->socket->Connect(socketId, ipAddress, portID);
}

int SysSendSocket(int socketId, char *buffer, int len)
{
    return kernel->socket->Send(socketId, buffer);
}

int SysReceiveSocket(int socketId, char *buffer)
{
    return kernel->socket->Receive(socketId, buffer);
}

int SysCloseSocket(int socketId)
{
    return kernel->socket->Close(socketId);
}

int SysExec(char *name)
{
    // cerr << "call: `" << name  << "`"<< endl;
    OpenFile *oFile = kernel->fileSystem->Open(name);
    if (oFile == NULL)
    {
        DEBUG(dbgSys, "\nExec:: Can't open this file.");
        return -1;
    }

    delete oFile;

    // Return child process id
    return kernel->pTab->ExecUpdate(name);
}

int SysJoin(int id) { return kernel->pTab->JoinUpdate(id); }

int SysExit(int id) { return kernel->pTab->ExitUpdate(id); }

#endif /* ! __USERPROG_KSYSCALL_H__ */
