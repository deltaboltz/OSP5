/* Author:      Connor Schultz
 * Created:     October 15, 2020
 * Last edit:   November 5, 2020
 */

#include <iostream>
#include <unistd.h>
#include <signal.h>

//includes of custom header files
#include "sharedmemory.h"
#include "error_handler.h"
#include "file_handler.h"
#include "clock_work.h"
#include "resource_handler.h"
#include "util.h"

volatile bool earlyquit = false;

//interrupt handler
void signalhandler(int signum)
{
    exit(-1);
}

int main(int argc, char **argv)
{

    signal(SIGINT, signalhandler);

    setupprefix(argv[0]);

    srand(getpid());//get a random pid

    int pid = std::stoi(argv[1]);

    //create the shared memory relative to the memory set by oss
    //0: shared clock
    //2: shared descriptors (resource manager)
    //3: shared system max of resource (used for calculation of requests and max claims)
    clk* shclk = (clk*)shmlookup(0);
    Descriptor* desc = (Descriptor*)shmlookup(2);
    int* sysmax = (int*)shmlookup(3);

    //set the start time of the process to the time of the shared clock
    float startTime = shclk->tofloat();
    long actConstant = 10e6;
    float nextActionTime = shclk->nextrand(actConstant);
    float termActionTime;

    bool maybeTerm = false; //assume that we may not terminate yet
    int reqChance = 55; //request chance is 55%
    int termChance = 4; //termination chance is 25%

    pcbmsgbuf* buf = new pcbmsgbuf;
    buf->mtype = 1; //mtype will always be set to 1 since we will only be sending messages to oss (mtype = 1)
    buf->data.pid = pid; //set the new pid to the child pid
    buf->data.status = CLAIM; //our first status will always be to state a max claim

    //loop through to set our max claim for this child process
    for (int i : range(20))
    {
        buf->data.resarray[i] = rand() % sysmax[i];
    }

    //send the message of what our max claim will be
    msgsend(1, buf);
    //what for a message to be sent back that we did set the max claim
    msgreceive(1, pid+2);

    //we no longer need the system max so we can detach from this early
    shmdetach(sysmax);

    while(1)
    {
        if (shclk->tofloat() >= startTime + 1 && !maybeTerm) //if the time we started to the present sim time is 1 second then we can determine if we will terminate
        {
            maybeTerm = true; //set to that we may terminate at any moment
            termActionTime = shclk->nextrand(250e6); //next time we see if we want to term (max 250 milliseconds)
        }

        if (maybeTerm && shclk->tofloat() >= termActionTime) //calculates our possiblity of terminating
        {
            if (rand() % termChance == 0) //if our random chance is 0 then we terminate
            {
                buf->data.status = TERM;
                buf->data.realpid = getpid();

                //if we do terminate then we need to detach from all memory
                shmdetach(shclk);
                msgsend(1, buf); //send oss a message that we did terminate
                shmdetach(desc);

                exit(0);
            }
            termActionTime = shclk->nextrand(250e6); //decide our next terminate status
        }

        if (shclk->tofloat() >= nextActionTime)
        {
            if (rand() % 100 < reqChance) //sets our % chance to do a request
            {
                int reqi = -1;
                bool requestable = false;

                //loop through to see if we can request any resource still (if we haven't met our max claim)
                for (int i : range(20))
                    if (desc[i].claim[pid] - desc[i].alloc[pid] > 0) requestable = true;

                //if we can't request anything then set our next action time
                if (!requestable)
                {
                    nextActionTime = shclk->nextrand(actConstant);
                    continue;
                }

                //if we can request, loop through to set what we want to request and how much of that resource we would like
                while (requestable && reqi < 0)
                {
                    int resi = rand() % 20;

                    if (desc[resi].claim[pid] - desc[resi].alloc[pid] > 0)
                    {
                        reqi = resi;
                    }
                }

                int reqamount = 1 + rand() % (desc[reqi].claim[pid] - desc[reqi].alloc[pid]);

                //send a message to oss that we are now requesting and how much we want to request
                buf->data.status = REQ;
                buf->data.resi = reqi;
                buf->data.resamount = reqamount;

                msgsend(1, buf);
                msgreceive(1, pid+2);
                //--------------------------------------------------------------------------------
            }
            else //if we didn't meet our chance to request then we will release a resource
            {
                int reli = -1;
                bool releaseable = false;

                //loop through to find out which resources we may be able to release (any resource we have something of)
                for (int i : range(20))
                {
                    if (desc[i].alloc[pid])
                    {
                        releaseable = true;
                    }
                }

                if (!releaseable) //if nothing is releasable then set the next action time
                {
                    nextActionTime = shclk->nextrand(actConstant);
                    continue;
                }

                //if something is releasable then loop through to find which resource we want to release and how much
                while (releaseable && reli < 0)
                {
                    int resi = rand() % 20;
                    if (desc[resi].alloc[pid]) reli = resi;
                }

                int relamount = 1 + rand() % (desc[reli].alloc[pid]);

                //send oss a message that we are releasing a resource and how much of it
                buf->data.status = REL;
                buf->data.resi = reli;
                buf->data.resamount = relamount;

                msgsend(1, buf);
                msgreceive(1, pid+2);
                //----------------------------------------------------------------------
            }

            nextActionTime = shclk->nextrand(actConstant); //calculate our next action time
        }
    }
}
