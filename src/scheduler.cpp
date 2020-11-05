//
// Created by Connor on 10/29/2020.
//
#include <csignal>
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

    auto i = this->findInQueue(process);

    if(process->priority < 0)
    {
        this->queue[process->priority+1].push_back(process);
        this->blocked.erase(i);
    }
    else if(process->priority < 3)
    {
        this->queue[process->priority+1].push_back(process);
        this->queue[process->priority].erase(i);
    }

    process->priority++;
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

void scheduleproc(mlfq& schedq, clck* shclck, pcb* proc, int logID, int& concCount)
{
    shclck->increment(100 + rand() % 9901);
    int pcbnum = proc->PCBTABLEPOS;

    pcbmsgbuffer* msg = new pcbmsgbuffer;

    msg->mtype = pcbnum + 3;
    msg->data[PCBNUM] = pcbnum;
    msg->data[TIMESLICE] = schedq.quantum[proc->priority];

    writeline(logID, shclck->toString() + ": Scheduling process with PID: " + std::to_string(proc->pid) + " with time quantum of: " + std::to_string(msg->data[TIMESLICE]) + " NanoSeconds");

    msgsend(2, msg);

    msg->mtype = 1;

    msgreceive(2, msg);

    shclck->increment(proc->burstTime);
    proc->cpuTime += proc->burstTime;
    writeline(logID, shclck->toString() + " Message was received after " + std::to_string(proc->burstTime) + " NanoSeconds");

    if(msg->data[STATUS] == TERM)
    {
        writeline(logID, shclck->toString() + ": PID " + std::to_string(proc->pid) + " is terminating");

        schedq.toExpired(proc);
        waitforanychild(concCount);
    }
    else if(msg->data[STATUS] == RUN)
    {
        writeline(logID, shclck->toString() + ": Moving PID " + std::to_string(proc->pid) + " to queue: " + std::to_string(proc->priority + 1));
        schedq.movePriority(proc);
    }
    else if(msg->data[STATUS] == BLOCK)
    {
        writeline(logID, shclck->toString() + ": Moving PID " + std::to_string(proc->pid) + " to blocked queue");
        schedq.toBlocked(proc);
    }
    else if(msg->data[STATUS] == PREEMTED)
    {
        writeline(logID, shclck->toString() + "PID " + std::to_string(proc->pid) + " was preempted and is now moving to priority: " + std::to_string(proc->priority + 1));
        schedq.movePriority(proc);
    }

    delete msg;

}

bool mlfq::isActive()
{
    for(auto q : this->queue)
    {
        if(!q.empty())
        {
            return true;
        }
    }
    return false;
}

void summary(mlfq& schq, clck* shclk, int quittype, std::string logfile, int logid)
{
    // PRINT SUMMARY
    // log reason for termination
    std::string summary;

    if (quittype == SIGINT)
    {
        summary = shclk->toString() + ": Simulation terminated due to a SIGINT";
    }
    else if (quittype == SIGALRM)
    {
        summary = shclk->toString() + ": Simulation terminated due to reaching end of allotted time";
        ipc_cleanup();
    }
    else if (quittype == 0)
    {
        summary = shclk->toString() + ": Simulation terminated after 100 processes were created";
    }

    std::cout << summary << "\n";
    writeline(logid, summary);

    // Average CPU Times
    long long avgCpu = 0;
    for (auto proc : schq.expired)
    {
        avgCpu += proc->cpuTime;
    }
    avgCpu /= schq.expired.size();

    summary = "Average CPU Utilization per PID (ns): " + std::to_string(avgCpu);

    std::cout << summary << "\n";
    writeline(logid, summary);

    // Average Blocked Queue Times
    long long avgBlock = 0;
    for (auto proc : schq.expired)
    {
        avgBlock  += proc->blockTime;
    }
    avgBlock /= schq.expired.size();
    summary = "Average time in Blocked Queue per PID (ns): " + std::to_string(avgBlock);

    std::cout << summary << "\n";
    writeline(logid, summary);

    // Average Idle CPU
    summary = "Total CPU Idle Time (ns): " + std::to_string(schq.idleTime);

    std::cout << summary << "\n";
    writeline(logid, summary);

    std::cout << "For a complete simulation log, please see " << logfile << "\n";
}

