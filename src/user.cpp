//
// Created by Connor on 10/27/2020.
//

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <csignal>
#include <vector>
#include <bitset>

#include "childhandler.h"

#include "childhandler.h"
#include "errors.h"
#include "sharedmemory.h"
#include "filehandler.h"
#include "scheduler.h"
#include "clock_work.h"

volatile bool earlyquit = false;

void sigHandle(int signum)
{
    exit(-1);
}

int main(int argc, char **argv)
{
    signal(SIGINT, sigHandle);

    setupprefix(argv[0]);

    srand(getpid());

    clck* shclock = (clck*)shmlook(0);
    pcb* pcbtable = (pcb*)shmlook(1);

    int pcbnum = std::stoi(argv[1]);
    std::cout << "PCB #" << pcbnum << " started\n";

    pcbmsgbuffer* msg = new pcbmsgbuffer;

    while(1)
    {
        msg->mtype = pcbnum + 3;
        msgreceive(2, msg);

        if(msg->data[PCBNUM] != pcbnum)
        {
            std::cerr << "received message not intended for child " << std::to_string(msg->data[PCBNUM]);
            perrorquit();
        }

        if(pcbtable[pcbnum].blockStart != 0)
        {
            pcbtable[pcbnum].blockTime += shclock->clockSec * 1e9 + shclock->clockNano - pcbtable[pcbnum].blockStart - 0;

            pcbtable[pcbnum].blockStart = 0;
        }

        msg->mtype = 1;

        if((rand() % 10) < 1)
        {
            pcbtable[pcbnum].burstTime = rand() % msg->data[TIMESLICE];
            msg->data[STATUS] = TERM;

            msgsend(2, msg);

            pcbtable[pcbnum].sysTime = pcbtable[pcbnum].burstTime + shclock->clockSec * 1e9 + shclock->clockNano - pcbtable[pcbnum].inceptTime;

            shmdetach(shclock);
            shmdetach(pcbtable);
            exit(0);
        }
        else
        {
            int decision = rand() % 3;
            if(decision == 0)
            {
                pcbtable[pcbnum].burstTime = msg->data[TIMESLICE];
                msg->data[STATUS] = RUN;
                msgsend(2, msg);
            }
            else if(decision == 1)
            {
                pcbtable[pcbnum].burstTime = rand() % msg->data[TIMESLICE];
                msg->data[STATUS] = BLOCK;
                msgsend(2, msg);
                pcbtable[pcbnum].blockStart = shclock->clockSec * 1e9 + shclock->clockNano;

                float wakeuptime = shclock->nextrand(3e9);

                while(shclock->tofloat() < wakeuptime);

                msg->mtype = 2;
                msg->data[STATUS] = UNBLOCK;
                msgsend(2, msg);

                msg->mtype = pcbnum+3;
            }
            else if(decision == 2)
            {
                pcbtable[pcbnum].burstTime = msg->data[TIMESLICE] / 100 * (1 + rand() % 99);
                msg->data[STATUS] = PREEMTED;
                msgsend(2, msg);
            }
        }
    }
}
