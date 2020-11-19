/* Author:      Connor Schultz
 * Created:     October 15, 2020
 * Last edit:   November 5, 2020
 */

#include <iostream>
#include <unistd.h>
#include <signal.h>
#include "sharedmemory.h"
#include "error_handler.h"
#include "file_handler.h"
#include "clock_work.h"
#include "resource_handler.h"
#include "util.h"

volatile bool earlyquit = false;

void signalhandler(int signum)
{
    exit(-1);
}

int main(int argc, char **argv)
{

    signal(SIGINT, signalhandler);

    setupprefix(argv[0]);

    srand(getpid());

    int pid = std::stoi(argv[1]);

    clk* shclk = (clk*)shmlookup(0);
    Descriptor* desc = (Descriptor*)shmlookup(2);
    int* sysmax = (int*)shmlookup(3);

    float startTime = shclk->tofloat();
    long actConstant = 10e6;
    float nextActionTime = shclk->nextrand(actConstant);
    float termActionTime;

    bool maybeTerm = false;
    int reqChance = 55;
    int termChance = 4;

    pcbmsgbuf* buf = new pcbmsgbuf;
    buf->mtype = 1;
    buf->data.pid = pid;
    buf->data.status = CLAIM;

    for (int i : range(20))
    {
        buf->data.resarray[i] = rand() % sysmax[i];
    }

    msgsend(1, buf);
    msgreceive(1, pid+2);
    shmdetach(sysmax);

    while(!0)
    {
        if (shclk->tofloat() >= startTime + 1 && !maybeTerm)
        {
            maybeTerm = true;
            termActionTime = shclk->nextrand(250e6);
        }

        if (maybeTerm && shclk->tofloat() >= termActionTime)
        {
            if (rand() % termChance == 0)
            {
                buf->data.status = TERM;
                buf->data.realpid = getpid();

                shmdetach(shclk);
                msgsend(1, buf);
                shmdetach(desc);

                exit(0);
            }
            termActionTime = shclk->nextrand(250e6);
        }

        if (shclk->tofloat() >= nextActionTime)
        {
            if (rand() % 100 < reqChance)
            {
                int reqi = -1;
                bool requestable = false;

                for (int i : range(20))
                    if (desc[i].claim[pid] - desc[i].alloc[pid] > 0) requestable = true;

                if (!requestable)
                {
                    nextActionTime = shclk->nextrand(actConstant);
                    continue;
                }
                while (requestable && reqi < 0)
                {
                    int resi = rand() % 20;

                    if (desc[resi].claim[pid] - desc[resi].alloc[pid] > 0)
                    {
                        reqi = resi;
                    }
                }

                int reqamount = 1 + rand() % (desc[reqi].claim[pid] - desc[reqi].alloc[pid]);

                buf->data.status = REQ;
                buf->data.resi = reqi;
                buf->data.resamount = reqamount;

                msgsend(1, buf);
                msgreceive(1, pid+2);
            }
            else
            {
                int reli = -1;
                bool releaseable = false;

                for (int i : range(20))
                {
                    if (desc[i].alloc[pid])
                    {
                        releaseable = true;
                    }
                }

                if (!releaseable)
                {
                    nextActionTime = shclk->nextrand(actConstant);
                    continue;
                }

                while (releaseable && reli < 0)
                {
                    int resi = rand() % 20;
                    if (desc[resi].alloc[pid]) reli = resi;
                }

                int relamount = 1 + rand() % (desc[reli].alloc[pid]);

                buf->data.status = REL;
                buf->data.resi = reli;
                buf->data.resamount = relamount;

                msgsend(1, buf);
                msgreceive(1, pid+2);
            }

            nextActionTime = shclk->nextrand(actConstant);
        }
    }
}
