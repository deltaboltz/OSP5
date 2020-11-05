//
// Created by connor on 10/26/2020.
//

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <string>
#include <cstring>
#include <set>
#include <errno.h>
#include <cerrno>
#include "errors.h"
#include "sharedmemory.h"
#include "filehandler.h"

std::set<int> shmsegments;
std::set<int> semaphores;
std::set<int> msgqueues;

struct msgbuffer
{
    long mtype;
};

key_t getkeyfromid(int keyID)
{
    key_t key = ftok(".", keyID);
    if (key == -1)
    {
        perrorquit();
    }
    return key;
}

int shmlookID(int keyID)
{
    int shmid = shmget(getkeyfromid(keyID), 0, 0);
    if (shmid == -1)
    {
        perrorquit();
    }
    return shmid;
}


void* shmlook(int keyID)
{
    void* ataddr = shmat(shmlookID(keyID), NULL, 0);
    if(ataddr == (void*)-1)
    {
        perrorquit();
    }
    return ataddr;
}

void* shmcreate(size_t bytes, int& keyID)
{
    int shmid = shmget(getkeyfromid(keyID++), bytes, IPC_CREAT|IPC_EXCL|0660);

    if(shmid == -1)
    {
        perrorquit();
    }
    shmsegments.insert(shmid);

    void* ataddr = shmat(shmid, NULL, 0);

    if(ataddr == ((void*)-1))
    {
        perrorquit();
    }
    return ataddr;
}

void shmdetach(const void* ptr)
{
    if(shmdt(ptr) == -1)
    {
        perrorquit();
    }
}

void shmdestroy(int keyID)
{
    if(shmctl(shmlookID(keyID), IPC_RMID, NULL) == -1)
    {
        perrorquit();
    }
}

int semcreate(int num, int& keyID)
{
    int semID = semget(getkeyfromid(keyID++), num, IPC_CREAT|IPC_EXCL|0660);

    if(semID == -1)
    {
        perrorquit();
    }

    semaphores.insert(semID);

    return semID;
}

int semlook(int keyID)
{
    int semID = semget(getkeyfromid(keyID), 0, 0);

    if(semID == -1)
    {
        perrorquit();
    }

    return semID;
}

void semlock(int semID, int semNum)
{
    struct sembuf op;
    op.sem_num = 1;
    op.sem_flg = 0;

    if(semop(semID, &op, 1) == -1)
    {
        perrorquit();
    }
}

void semunlock(int semID, int semNum)
{
    struct sembuf op;
    op.sem_num = semNum;
    op.sem_op = 1;
    op.sem_flg = 0;

    if(semop(semID, &op, 1) == -1)
    {
        perrorquit();
    }
}

void semlockall(int semID, int semSize)
{
    struct sembuf op[semSize];

    for(int i = 0; i < semSize; i++)
    {
        op[i].sem_num = i;
        op[i].sem_op = -1;
        op[i].sem_flg = 0;
    }

    if(semop(semID, op, semSize) == -1)
    {
        perrorquit();
    }
}

void semdestroy(int semID)
{
    if(semctl(semID, 0, IPC_RMID) == -1)
    {
        perrorquit();
    }
}

void msgcreate(int& keyID)
{
    int msqID = msgget(getkeyfromid(keyID++), IPC_CREAT|IPC_EXCL|0660);

    if(msqID == -1)
    {
        perrorquit();
    }

    msgqueues.insert(msqID);
}

int msggetID(int keyID)
{
    int msqID;
    if((msqID = msgget(getkeyfromid(keyID), 0660)) == -1)
    {
        perrorquit();
    }
    return msqID;
}

void msgsend(int keyID)
{
    struct msgbuffer buf;
    buf.mtype = 1;

    if(msgsnd(msggetID(keyID), &buf, 0 ,0) == -1)
    {
        perrorquit();
    }
}

void msgsend(int keyID, int mtype)
{
    struct msgbuffer buf;
    buf.mtype = mtype;

    if(msgsnd(msggetID(keyID), &buf, 0 ,0) == -1)
    {
        perrorquit();
    }
}

void msgsend(int keyID, pcbmsgbuffer* buf)
{
    if (msgsnd(msggetID(keyID), buf, sizeof(buf->data), 0) == -1)
    {
        perrorquit();
    }
}

void msgreceive(int keyID)
{
    struct msgbuffer buf;

    if(msgrcv(msggetID(keyID), &buf, 0, 0, 0) == -1)
    {
        perrorquit();
    }
}

void msgreceive(int keyID, int mtype)
{
    struct msgbuffer buf;
    if (msgrcv(msggetID(keyID), &buf, 0, mtype, 0) == -1 && errno != EINTR)
    {
        perrorquit();
    }
}

void msgreceive(int key_id, pcbmsgbuffer* buf) {
    if (msgrcv(
        msggetID(key_id), buf, sizeof(buf->data), buf->mtype, 0) == -1 && errno != EINTR)
            perrorquit();
}

bool msgreceivenw(int keyID)
{
    struct msgbuffer buf;

    if(msgrcv(msggetID(keyID), &buf, 0, 0, IPC_NOWAIT) == -1 && errno != EINTR)
    {
        if(errno != ENOMSG)
        {
            perrorquit();
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool msgreceivenw(int keyID, pcbmsgbuffer* buf)
{
    if(msgrcv(msggetID(keyID), buf, sizeof(buf->data), buf->mtype, IPC_NOWAIT) == -1)
    {
        if(errno != ENOMSG)
        {
            perrorquit();
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool msgrecwithdatanw(int keyID, int pcbnum)
{
    struct pcbmsgbuffer* buf = new pcbmsgbuffer;

    buf->data[2] = -1;

    if(msgrcv(msggetID(keyID), buf, sizeof(buf->data), pcbnum+2, IPC_NOWAIT) == -1 && errno != ENOMSG)
    {
        perrorquit();
    }

    return buf;
}

void msgdestroy(int keyID)
{
    if(msgctl(msggetID(keyID), IPC_RMID, NULL) == -1)
    {
        perrorquit();
    }
}

void ipc_cleanup()
{
    for(int shmid : shmsegments)
    {
        if(shmctl(shmid, IPC_RMID, NULL) == -1)
        {
            perrorquit();
        }
    }
    for(int semid : semaphores)
    {
        if(semctl(semid, 0, IPC_RMID) == -1)
        {
            perrorquit();
        }
    }
    for(int msqID : msgqueues)
    {
        if(msgctl(msqID, IPC_RMID, NULL) == -1)
        {
            perrorquit();
        }
    }
}






