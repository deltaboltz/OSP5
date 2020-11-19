/* Author:      Connor Schultz
 * Created:     October 6, 2020
 * Last edit:   November 5, 2020
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <string>
#include <cstring>
#include <set>
#include "error_handler.h"
#include "file_handler.h"
#include "util.h"
#include "sharedmemory.h"


static std::set<int> shmsegments;
static std::set<int> semaphores;
static std::set<int> msgqueues;

struct msgbuffer
{
    long mtype;
};

key_t getkeyfromid(int key_id)
{
    key_t key = ftok(".", key_id);
    if (key == -1) perrandquit();
    return key;
}

int shmlookupid(int key_id)
{
    int shmid = shmget(getkeyfromid(key_id), 0, 0);
    if (shmid == -1) perrandquit();
    return shmid;
}

void* shmcreate(size_t bytes, int& key_id)
{
    int shmid = shmget(getkeyfromid(key_id++), bytes, IPC_CREAT|IPC_EXCL|0660);

    if (shmid == -1) perrandquit();

    shmsegments.insert(shmid);

    void* ataddr = shmat(shmid, NULL, 0);

    if (ataddr == (void*)-1) perrandquit();

    return ataddr;
}

void* shmlookup(int key_id)
{
    void* ataddr = shmat(shmlookupid(key_id), NULL, 0);
    if (ataddr == (void*)-1) perrandquit();

    return ataddr;
}

void shmdetach(const void* shmptr)
{
    if (shmdt(shmptr) == -1) perrandquit();
}

void shmdestroy(int key_id)
{
    if (shmctl(shmlookupid(key_id), IPC_RMID, NULL) == -1) perrandquit();
}

int semcreate(int num, int& key_id)
{

    int semid = semget(getkeyfromid(key_id++), num, IPC_CREAT|IPC_EXCL|0660);

    if (semid == -1) perrandquit();

    semaphores.insert(semid);

    return semid;
}

int semlookup(int key_id)
{
    int semid = semget(getkeyfromid(key_id), 0, 0);
    if (semid == -1) perrandquit();

    return semid;
}

void semlock(int semid, int semnum)
{
    struct sembuf op;
    op.sem_num = semnum;
    op.sem_op = -1;
    op.sem_flg = 0;
    if (semop(semid, &op, 1) == -1 && errno != 4)
    {
        perrandquit();
    }
}

void semunlock(int semid, int semnum)
{
    struct sembuf op;
    op.sem_num = semnum;
    op.sem_op = 1;
    op.sem_flg = 0;

    if (semop(semid, &op, 1) == -1) perrandquit();
}

void semunlockall(int semid, int semsize)
{


    struct sembuf op[semsize];

    for ( int i = 0; i < semsize; i++)
    {
        op[i].sem_num = i;
        op[i].sem_op = 1;
        op[i].sem_flg = 0;
    }

    if (semop(semid, op, semsize) == -1) perrandquit();
}

void semlockall(int semid, int semsize)
{
    struct sembuf op[semsize];
    for (int i : range(semsize))
    {
        op[i].sem_num = i;
        op[i].sem_op = -1;
        op[i].sem_flg = 0;
    }

    if (semop(semid, op, semsize) == -1) perrandquit();
}

void semdestroy(int semid)
{
    if (semctl(semid, 0, IPC_RMID) == -1) perrandquit();
}

void msgcreate(int& key_id)
{

    int msqid = msgget(getkeyfromid(key_id++), IPC_CREAT|IPC_EXCL|0660);

    if (msqid == -1) perrandquit();

    msgqueues.insert(msqid);
}

int msglookupid(int key_id)
{
    int msqid = msgget(getkeyfromid(key_id), 0660);
    if (msqid == -1) perrandquit();
    return msqid;
}

void msgsend(int key_id)
{
    struct msgbuffer buf;
    buf.mtype = 1;
    if (msgsnd(msglookupid(key_id), &buf, 0, 0) == -1) perrandquit();
}

void msgsend(int key_id, int mtype)
{
    struct msgbuffer buf;
    buf.mtype = mtype;
    if (msgsnd(msglookupid(key_id), &buf, 0, 0) == -1) perrandquit();
}

void msgsend(int key_id, pcbmsgbuf* buf)
{
    if (msgsnd(msglookupid(key_id), buf, sizeof(buf->data), 0) == -1)
        perrandquit();
}

void msgreceive(int key_id)
{
    struct msgbuffer buf;


    if (msgrcv(msglookupid(key_id), &buf, 0, 0, 0) == -1 && errno != EINTR)
        perrandquit();
}

void msgreceive(int key_id, int mtype)
{
    struct msgbuffer buf;

    if (msgrcv(msglookupid(key_id), &buf, 0, mtype, 0) == -1 && errno != EINTR)
        perrandquit();
}

void msgreceive(int key_id, pcbmsgbuf* buf)
{

    if (msgrcv(
        msglookupid(key_id), buf, sizeof(buf->data), buf->mtype, 0) == -1 &&
            errno != EINTR)
                perrandquit();
}

bool msgreceivenw(int key_id)
{
    struct msgbuffer buf;

    if (msgrcv(msglookupid(key_id), &buf, 0, 0, IPC_NOWAIT) == -1)
    {
        if (errno != ENOMSG)
        {
            perrandquit();
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool msgreceivenw(int key_id, int mtype)
{
    struct msgbuffer buf;

    if (msgrcv(msglookupid(key_id), &buf, 0, mtype, IPC_NOWAIT) == -1)
    {
        if (errno != ENOMSG)
        {
            perrandquit();
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool msgreceivenw(int key_id, pcbmsgbuf* buf)
{
    if (msgrcv(msglookupid(key_id), buf, sizeof(buf->data),
               buf->mtype, IPC_NOWAIT) == -1)
    {
        if (errno != ENOMSG)
        {
            perrandquit();
        }
        else
        {
            return false;
        }
    }

    return true;
}

void msgdestroy(int key_id)
{
    if (msgctl(msglookupid(key_id), IPC_RMID, NULL) == -1) perrandquit();
}

void ipc_cleanup()
{
    for (int shmid : shmsegments)
        if (shmctl(shmid, IPC_RMID, NULL) == -1) perrandquit();

    for (int semid : semaphores)
        if (semctl(semid, 0, IPC_RMID) == -1) perrandquit();

    for (int msqid : msgqueues)
        if (msgctl(msqid, IPC_RMID, NULL) == -1) perrandquit();

    shmsegments.clear();
    semaphores.clear();
    msgqueues.clear();
}
