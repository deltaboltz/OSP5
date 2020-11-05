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

void mainloop(int concurrent, std::string filename)
{
    int maxCount = 0;
    int concCount = 0;
    int currentID = 0;
    int logID = addout_append(filename);
    int maxBTSs = 2;
    int maxBTSn = 0;

    srand(getpid());

    clck* shclock = (clck*)shmcreate(sizeof(clck), currentID);
    float nextSpawnTime = shclock->nextrand(maxBTSs * 1e9 + maxBTSn);

    mlfq schedq;
    schedq.pcbtable = (pcb*)shmcreate(sizeof(pcb)*18, currentID);
    msgcreate(currentID);
    pcb* proc;

    while(!earlyquit) {
        if (schedq.isEmpty()) {
            if (maxCount >= 100) {
                earlyquit = true;
                quitType = 0;
            } else {
                if (shclock->tofloat() < nextSpawnTime) {
                    shclock->set(nextSpawnTime);
                }
            }
        }

        if (!schedq.isActive() && schedq.idleTime == 0) {
            schedq.idleStart = shclock->clockSec * 1e9 + shclock->clockNano;
        }

        if (!schedq.blocked.empty()) {
            unblockproc(schedq, shclock, logID);
        }

        int pcbnum = -1;

        if ((maxCount < 100) && (shclock->tofloat() >= nextSpawnTime) && ((pcbnum = schedq.addProc()) != -1)) {
            std::cout << "Bitmap: " << schedq.bitmap << "\n";
            proc = &schedq.pcbtable[pcbnum];

            forkexec("user " + std::to_string(pcbnum), concCount);

            proc->inceptTime = shclock->clockSec * 1e9 + shclock->clockNano;

            writeline(logID, shclock->toString() + ": Spawing PID" + std::to_string(proc->pid) + " (" +
                             std::to_string(++maxCount) + "/100)");

            nextSpawnTime = shclock->nextrand(maxBTSs * 1e9 + maxBTSn);

            if (maxCount < 100) {
                writeline(logID, "\tNext spawn will be @: " + std::to_string(nextSpawnTime));
            }
        }

        if ((proc = schedq.getFirst()) != NULL) {
            scheduleproc(schedq, shclock, proc, logID, concCount);
        }

        shclock->increment(rand() % (long) 1e9);


        if ((proc = schedq.getFirst()) != NULL) {
            if (schedq.idleStart) {
                schedq.idleTime += shclock->clockSec * 1e9 + shclock->clockNano - schedq.idleStart;
                schedq.idleStart = 0;
            }
            scheduleproc(schedq, shclock, proc, logID, concCount);
        }
        shclock->increment(rand() % (long) 1e9);
    }



    summary(schedq, shclock, quitType, filename, logID);

    //avg Idle CPU

    while (concCount > 0)
    {
        killall();
        updatechildcount(concCount);
    }

    shmdetach(shclock);
    shmdetach(schedq.pcbtable);
    ipc_cleanup();

}

int main(int argc, char **argv)
{
    signal(SIGINT, sigHandle);
    signal(SIGALRM, sigHandle);
    setupprefix(argv[0]);

    int concurrent = 18;

    std::string filename = "output-" + epochlogid() + ".log";

    int MAXTIME = 3;
    alarm(MAXTIME);

    mainloop(concurrent, filename);

    return 0;
}


