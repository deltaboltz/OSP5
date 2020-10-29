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

struct pcb
{
    float cpuClock;
    float sysClock;
    float burst;
    int pid;
    int priority;
    int PCBTABLEPOS;

    pcb(int& extPid, int position)
    {
        cpuClock = 0;
        sysClock = 0;
        burst = 0;
        pid = extPid;
        priority = 0;
        PCBTABLEPOS = position;
    }

    pcb(const pcb &old)
    {
        cpuClock = old.cpuClock;
        sysClock = old.sysClock;
        burst = old.burst;
        pid = old.pid;
        priority = old.priority;
        PCBTABLEPOS = old.PCBTABLEPOS;
    }
};

struct mlfq
{
    pcb* pcbtable;
    std::list<pcb*> blocked;
    std::list<pcb*> queue[4];
    std::list<pcb*> expired;
    std::bitset<18> bitmap;

    int T1 = 10000;
    int T2 = 20000;
    int T3 = 40000;
    int T4 = 80000;
    int pid = 0;

    bool isEmpty();
    pcb* getFirst();
    void movePriority(pcb* process);
    void toBlocked(pcb* process);
    void toExpired(pcb* process);
    std::list<pcb*>::iterator findInQueue(pcb* process);
    void printQueues();
    int addProc();
};



#endif
