//
// Created by Connor on 10/29/2020.
//
#include "scheduler.h"
#include "childhandler.h"
#include "filehandler.h"
#include "clock_work.h"
#include "sharedmemory.h"


template<size_t T> int findfirstunset(std::bitset<T> bs)
{
    for(size_t i = 0; i < T; i++)
    {
        if(!bs[i])
        {
            return i;
        }
    }
    return -1;
}

bool mlfq::isEmpty()
{
    if(!(this->blocked).empty())
    {
        return false;
    }

    for(auto q : this->queue)
    {
        if(!q.empty())
        {
            return false;
        }
    }

    return true;
}

int mlfq::addProc()
{
    int index = findfirstunset(this->bitmap);

    if(index != -1)
    {
        pcb* newProc = new pcb(this->pid, index);

        this->pcbtable[index] = *newProc;
        delete newProc;

        this->queue[0].push_back(&(this->pcbtable[index]));
        this->bitmap.set(index);
    }

    return index;
}



pcb* mlfq::getFirst()
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

void mlfq::movePriority(pcb *process)
{
    if(process->priority == 3)
    {
        return;
    }
    std::cout << "1" << std::endl;

    auto i = this->findInQueue(process);

    if(process->priority < 0)
    {
        std::cout << "2" << std::endl;

        this->queue[process->priority+1].push_back(process);
        this->blocked.erase(i);
        std::cout << "3" << std::endl;

    }
    else if(process->priority < 3)
    {
        std::cout << "4" << std::endl;

        this->queue[process->priority+1].push_back(process);
        this->queue[process->priority].erase(i);
        std::cout << "5" << std::endl;

    }
    std::cout << "6" << std::endl;

    process->priority++;

    std::cout << "7" << std::endl;

}



void mlfq::toBlocked(pcb *process)
{
    auto i = this->findInQueue(process);

    this->blocked.push_back(process);
    this->queue[process->priority].erase(i);

    process->priority = -1;
}

void mlfq::toExpired(pcb *process)
{
    auto i = this->findInQueue(process);

    this->expired.push_back(new pcb(*process));

    if(process->priority < 0)
    {
        this->blocked.erase(i);
    }
    else
    {
        this->queue[process->priority].erase(i);
    }

    this->bitmap.reset(process->PCBTABLEPOS);
}

std::list<pcb*>::iterator mlfq::findInQueue(pcb *process)
{
    int PRI = process->priority;
    std::list<pcb*>::iterator i;

    if(PRI < 0)
    {
        i = this->blocked.begin();

        while(i != this->blocked.end() && (*i) != process)
        {
            i++;
        }
    }
    else if(PRI < 4)
    {
        i = this->queue[PRI].begin();

        while(i != this->queue[PRI].end() && (*i) != process)
        {
            i++;
        }
    }
    else
    {
        i = this->expired.begin();

        while(i != this->expired.end() && (*i) != process)
        {
            i++;
        }
    }

    if((*i) != process)
    {
        perrorquit();
    }
    return i;
}

void mlfq::printQueues()
{

    auto proc = this->blocked.begin();
    std::cout << "Blocked: ";
    for(; proc != this->blocked.end(); ++proc)
    {
        std::cout << "(" << (*proc)->pid << ", " << (*proc)->pid << ") " << std::endl;
    }

    std::cout << "\nPQ 1: ";
    for(auto proc : this->queue[0])
    {
        std::cout << "(" << proc->pid << ", " << proc->priority << ") " << std::endl;
    }

    std::cout << "\nPQ 2: ";
    for(auto proc : this->queue[1])
    {
        std::cout << "(" << proc->pid << ", " << proc->priority << ") " << std::endl;
    }

    std::cout << "\nPQ 3: ";
    for(auto proc : this->queue[2])
    {
        std::cout << "(" << proc->pid << ", " << proc->priority << ") " << std::endl;
    }

    std::cout << "\nPQ 4: ";
    for(auto proc : this->queue[3])
    {
        std::cout << "(" << proc->pid << ", " << proc->priority << ") " << std::endl;
    }
}

void unblockproc(mlfq& schedq, clck* shclck, int logID)
{
    pcbmsgbuffer* msg = new pcbmsgbuffer;

    msg->mtype = 2;

    if(msgreceivenw(2, msg))
    {
        shclck->increment(1000 + rand() % 19901);
        pcb* proc = &schedq.pcbtable[msg->data[PCBNUM]];

        writeline(logID, shclck->toString() + ": Unblocking PID " + std::to_string(proc->pid));

        schedq.movePriority(proc);
    }

    delete msg;
}
