#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <csignal>
#include <vector>
#include <bitset>

#include "childhandler.h"
#include "errors.h"
#include "sharedmemory.h"
#include "filehandler.h"
#include "scheduler.h"
#include "clock_work.h"

volatile bool earlyquit = false;
volatile bool interrupt = false;
volatile int quitType = 0;

void sigHandle(int signum)
{
    if(signum == SIGALRM || signum == SIGINT)
    {
        earlyquit = true;
        quitType = signum;
    }
}

void earlyQuitHandle()
{
    if(!interrupt)
    {
        interrupt = true;
        killall();

        if(quitType == SIGINT)
        {
            std::cerr << "SIGINT rcvd. QUITING" << std::endl;
        }
        else if(quitType == SIGALRM)
        {
            std::cerr << "Time limit reached. QUITING" << std::endl;
        }
    }
}

void mainloop(int concurrent, const char* filename)
{
    int MAXPROC = 0;
    int concNum = 0;
    int currentID = 0;
    int nextPID = 0;
    int logID = addout_append(filename);
    int MAXWAIT_SEC = 2;
    int MAXWAIT_NANO = 0;
    float nextSpawn;

    clck* shclock = (clck*)shmcreate(sizeof(clck), currentID);

    mlfq scheduleQ;
    scheduleQ.pcbtable = (pcb*)shmcreate(sizeof(pcb)*18, currentID);

    scheduleQ.addProc();
    scheduleQ.addProc();
    scheduleQ.addProc();
    scheduleQ.addProc();

    scheduleQ.movePriority(&scheduleQ.pcbtable[0]);
    scheduleQ.movePriority(&scheduleQ.pcbtable[0]);
    scheduleQ.movePriority(scheduleQ.getFirst());

    scheduleQ.toBlocked(&scheduleQ.pcbtable[3]);

    scheduleQ.movePriority(&scheduleQ.pcbtable[2]);
    scheduleQ.movePriority(&scheduleQ.pcbtable[2]);
    scheduleQ.movePriority(&scheduleQ.pcbtable[2]);

    scheduleQ.toExpired(&scheduleQ.pcbtable[2]);

    scheduleQ.printQueues();

    shmdetach(shclock);
    shmdetach(pcbtable);
    ipc_cleanup();

}

int main(int argc, char **argv)
{
    signal(SIGINT, sigHandle);
    signal(SIGALRM, sigHandle);
    setupprefix(argv[0]);

    int concurrent = 18;

    const char* filename = "out.log";
    int MAXTIME = 3;

    alarm(MAXTIME);

    mainloop(concurrent, filename);
}


