//
// Created by Connor on 10/26/2020.
//

#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

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
void msgreceive(int keyID);
void ipc_cleanup();

#endif
