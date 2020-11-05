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

void mainloop(int concurrent, std::string filename)
{
    //Initialize variables for the mainloop function
    int maxCount = 0;
    int concCount = 0;
    int currentID = 0;
    int logID = addout_append(filename);
    int maxBTSs = 2;
    int maxBTSn = 0;

    //set random value for pid
    srand(getpid());

    //attach the shared clock to the oss process
    clck* shclock = (clck*)shmcreate(sizeof(clck), currentID);

    //set the nextSpawnTime as a random through the nextrand function in clockwork.h
    float nextSpawnTime = shclock->nextrand(maxBTSs * 1e9 + maxBTSn);

    //create the multi-level feedback queue
    mlfq schedq;
    //create the shared memory for the pcb table inside the mlfq
    schedq.pcbtable = (pcb*)shmcreate(sizeof(pcb)*18, currentID);

    //create the message queue
    msgcreate(currentID);

    //create the pcb linked process
    pcb* proc;

    //Run a while loop while there is no earlyquit signal sent
    while(!earlyquit)
    {
        //Check to see if the schedule queues are empty
        if (schedq.isEmpty())
        {
            //If they are then check to see if we have reached 100 processes
            if (maxCount >= 100)
            {
                //set earlyquit to true, leave the while loop
                earlyquit = true;
                quitType = 0;
            }
            else
            {
                if (shclock->tofloat() < nextSpawnTime)
                {
                    //If we aren't about to quit then set the next spawn time
                    shclock->set(nextSpawnTime);
                }
            }
        }

        //Check to see if there are any active processes in the queues or none at all
        if (!schedq.isActive() && schedq.idleStart == 0)
        {
            //Set out initial idle time to when this starts
            schedq.idleStart = shclock->clockSec * 1e9 + shclock->clockNano;
        }

        //If the schedule queue for blocked is NOT empty then unblock a process
        if (!schedq.blocked.empty())
        {
            unblockproc(schedq, shclock, logID);
        }

        //set the pcbnumber, index, to -1 since this'll be reserved
        int pcbnum = -1;

        //If the max count of processes and the shared clock is past the due time for spawn AND adding a process doesn't
        //error out then we can create a new process
        if ((maxCount < 100) && (shclock->tofloat() >= nextSpawnTime) && ((pcbnum = schedq.addProc()) != -1)) {
            //std::cout << "Bitmap: " << schedq.bitmap << "\n"; //used for debugging

            //set the process in the pcbtable at the pcbnum index
            proc = &schedq.pcbtable[pcbnum];

            //fork for a new process
            forkexec("user " + std::to_string(pcbnum), concCount);

            //Set the inception time of the new process to the clock of shared memory
            proc->inceptTime = shclock->clockSec * 1e9 + shclock->clockNano;

            //Write to the logfile that we are now spawning a new child (x/100)
            writeline(logID, shclock->toString() + ": Spawing PID" + std::to_string(proc->pid) + " (" + std::to_string(++maxCount) + "/100)");

            //Set the next spawn time
            nextSpawnTime = shclock->nextrand(maxBTSs * 1e9 + maxBTSn);

            //Print the next spawn time IF we are still below 100 processes
            if (maxCount < 100)
            {
                writeline(logID, "\tNext spawn will be @: " + std::to_string(nextSpawnTime));
            }
        }

        //Check to see if the first pcb location is not empty
        if ((proc = schedq.getFirst()) != NULL)
        {
            //Check to see if the idleStart variable has a value or not
            if (schedq.idleStart)
            {
                //If it does then calculate the idleTime
                schedq.idleTime += shclock->clockSec * 1e9 + shclock->clockNano - schedq.idleStart;
                //std::cout << "CPU Idled for: " << std::to_string(schedq.idleTime) << std::endl; // used for debugging

                //reset the idleStart to 0
                schedq.idleStart = 0;
            }
            //Schedule a new process
            scheduleproc(schedq, shclock, proc, logID, concCount);
        }
        //increment the clock by a random value
        shclock->increment(rand() % (long) 1e9);
    }

    //Print the summary of the program
    summary(schedq, shclock, quitType, filename, logID);

    //If we still have children alive then wait for them to end
    while (concCount > 0)
    {
        killall();
        updatechildcount(concCount);
    }

    //Clean up out memory

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

    //Sets a unique logid no matter what
    std::string filename = "output-" + epochlogid() + ".log";

    //Max time before timeout
    int MAXTIME = 3;
    alarm(MAXTIME);

    //Call mainloop
    mainloop(concurrent, filename);

    return 0;
}


