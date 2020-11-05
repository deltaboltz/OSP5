//
// Created by Connor on 10/27/2020.
//

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <list>
#include <bitset>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include "errors.h"
#include "clock_work.h"

struct pcb
{
    long cpuTime;
    long sysTime;
    long burstTime;
    long inceptTime;

    long blockStart;
    long blockTime;

    int pid;
    int priority;

    int PCBTABLEPOS;

    pcb(int& extPid, int position)
    {
        cpuTime = 0;
        sysTime = 0;
        burstTime = 0;
        inceptTime = 0;

        blockStart = 0;
        blockTime = 0;

        pid = extPid++;
        priority = 0;

        PCBTABLEPOS = position;
    }

    pcb(const pcb &old)
    {
        cpuTime = old.cpuTime;
        sysTime = old.sysTime;
        burstTime = old.burstTime;
        inceptTime = old.inceptTime;

        blockStart = old.blockStart;
        blockTime = old.blockTime;

        pid = old.pid;
        priority = old.priority;

        PCBTABLEPOS = old.PCBTABLEPOS;
    }
};

struct mlfq
{
    pcb* pcbtable;

    std::list<pcb*> queue[4];
    std::list<pcb*> blocked;
    std::list<pcb*> expired;
    int quantum[4] = {10000, 20000, 40000, 80000};

    std::bitset<18> bitmap;

    int pid = 0;

    bool isEmpty();
    int addProc();

    pcb* getFirst();

    void movePriority(pcb* process);

    void toBlocked(pcb* process);
    void toExpired(pcb* process);

    std::list<pcb*>::iterator findInQueue(pcb* process);

    void printQueues();

};

void unblockproc(mlfq& schedq, clck* shclck, int logid);
void scheudleproc(mlfq& shedq, clck* shclck, pcb* proc, int logid, int& concCount);

#endif
