//
// Created by Connor on 10/26/2020.
//

#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include <sys/types.h>

enum Data {PCBNUM, TIMESLICE, STATUS};
enum Status {TERM, RUN, BLOCK, UNBLOCK};

struct pcbmsgbuffer
{
    long mtype;
    int data[3];
};

void* shmcreate(size_t bytes, int& keyID);
void* shmlook(int keyID);
void shmdetach(const void* ptr);
void shmdestroy(int keyID);


int semcreate(int num, int& keyID);
int semlook(int keyID);
void semlock(int semID, int semNum);
void semunlock(int semID, int semNum);
void semunlockall(int semID, int semSize);
void semlockall(int semID, int semSize);
void semdestroy(int semID);
void semdestroy(int semID);
void msgcreate(int& keyID);


int msggetID(int keyID);


void msgsend(int keyID);
void msgsend(int keyID, int mtype);
void msgsend(int keyID, pcbmsgbuffer* buf);

void msgreceive(int keyID);
void msgreceive(int keyID, int mtype);
void msgreceive(int keyID, pcbmsgbuffer* buf);

bool msgreceivenw(int keyID);
bool msgreceivenw(int keyID, int mtype);
bool msgreceivenw(int keyID, pcbmsgbuffer* buf);

void ipc_cleanup();

#endif
