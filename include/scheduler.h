//
// Created by Connor on 10/27/2020.
//

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <list>
#include <iostream>
#include <stdlib.h>

struct pcb
{
    float cpuClock;
    float sysClock;
    float burst;
    int pgid;
    int priority;
    bool realTimeProc;
    int realTimeChance = 1;

    pcb(int& extPid)
    {
        cpuClock = 0;
        sysClock = 0;
        burst = 0;
        pgid = extPid;
        priority = 0;
        realTimeProc = (rand() % 100 < realTimeChance);
    }
};

struct queues
{
    std::list<pcb*> blocked;
    std::list<pcb*> queue[4];
    std::list<pcb*> expired;

    int T1 = 10000;
    int T2 = 20000;
    int T3 = 40000;
    int T4 = 80000;

    bool isEmpty();
    pcb* getFirst();
    void movePriority(pcb* process);
};

bool queues::isEmpty()
{
    for(auto q : this->queue)
    {
        if(!q.empty())
        {
            return false;
        }
    }
    return true;
}

pcb* queues::getFirst()
{
    for(auto q : this->queue)
    {
        if(!q.empty())
        {
            return q.front();
        }
    }
    return NULL;
}

void queues::movePriority(pcb *process)
{
    int PRI = process->priority;

    if(PRI < 3)
    {
        auto i = this->queue[PRI].begin();
        for(; i != this->queue[PRI].end(); i++)
        {
            if((*i)->pgid == process->pgid)
            {
                break;
            }
        }

        if(i == this->queue[PRI].end() && (*i)->pgid != process->pgid)
        {
            std::cerr << "Cannot get PCB for PID: " << process->pgid;
            std::cerr << "in queue " << PRI << std::endl;
        }

        this->queue[PRI+1].push_back(process);
        this->queue[PRI].erase(i);
    }
}

#endif
