/* Author:      Connor Schultz
 * Created:     September 9, 2020
 * Last edit:   November 05, 2020
 */

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <csignal>
#include <vector>
#include <list>

//includes of all custom header files
#include "cmd_handler.h"
#include "childhandler.h"
#include "error_handler.h"
#include "sharedmemory.h"
#include "help_handler.h"
#include "clock_work.h"
#include "resource_handler.h"
#include "log_handler.h"
#include "util.h"


//all intterupt handlers
volatile bool earlyquit = false;
volatile bool handlinginterrupt = false;
volatile int quittype = 0;


void signalhandler(int signum) //catches any intterupt given either by the user or the system
{
    if (signum == SIGALRM || signum == SIGINT)
    {
        earlyquit = true;
        quittype = signum;
    }
}

void cleanup(clk* shclk, resman r, int& conc_count) //cleans up any memory that is remaining when the intterupt is called
{

    //loop through to kill all the remaining children in the system
    while (conc_count > 0)
    {
        killallchildren();
        updatechildcount(conc_count);
    }

    shmdetach(shclk);
    shmdetach(r.desc);
    shmdetach(r.sysmax);
    ipc_cleanup();
}

void testopts(int argc, char** argv, std::string pref, int optind, bool* flags) //checks to see whether proper flags were given if any were required
{
    if (flags[0])
    {
        printhelp(pref);
        exit(0);
    }

    if (argc > optind)
    {
      custerrhelpprompt("Unknown argument '" + std::string(argv[optind]) + "'");
    }
}

void unblockAfterRelease(clk* shclk, resman& r, std::list<Data>& blocked, Log& log) //unblocks a certain process if resources can now be allocated
{
    auto i = blocked.begin();

    //loop through the blocked list to find the pid to unblock after the resources were released into the system
    while (i != blocked.end())
    {
        if (r.allocate((*i).pid, (*i).resi, (*i).resamount) == 0)
        {
            //msgsnd must have an mytype of +2 since 0 cannot send and 1 is reserved for oss
            msgsend(1, (*i).pid+2);
            log.logUnblock(shclk, (*i));
            blocked.erase(i++);
        }
        else
        {
            i++;
        }
    }
}

void main_loop(std::string runpath, Log& log)
{
    int max_count = 0;
    int conc_count = 0;
    int currID = 0;
    int spawnConst = 500000000;
    int pid;
    int reqs = 0;

    srand(getpid());

    //create the shared clock object and set the next spawn time of a child inside the system
    clk* shclk = (clk*)shmcreate(sizeof(clk), currID);
    float nextSpawnTime = shclk->nextrand(spawnConst);

    //creates a message id with the currentID, first will be 0
    msgcreate(currID);

    //create a resource manager object with the currentID, first will be 0
    resman r(currID);
    pcbmsgbuf* buf = new pcbmsgbuf;

    std::list<Data> blockedRequests;

    //loop goes until an earlyquit signal is possibly sent
    while (!earlyquit)
    {
        //if we reach our max count AND our concurrent process count is 0 then set the earlyquit to true
        if (max_count >= 40 && conc_count == 0)
        {
          earlyquit = true;
        }

        //checks to see if the clock has reached the time for our next spawn AND we haven't spawned the max children allowed for the simulation
        if (shclk->tofloat() >= nextSpawnTime && max_count < 40)
        {
            //sets the pid inside of the bitmap
            r.findandsetpid(pid);

            //as long as the pid isn't 0 (oss) then we will log the clock, pid, and the max count to the log file
            if (pid >= 0)
            {
                log.logNewPID(shclk, pid, ++max_count);

                //fork the user.cpp file with a proper runpath (bin/user.cpp)
                forkexec(runpath + "user " + std::to_string(pid), conc_count);

                //create a new random spawn time for the next child
                nextSpawnTime = shclk->nextrand(spawnConst);
            }
        }

        //set the mtype to 1 for OSS to use
        buf->mtype = 1;

        //msgrcv with now wait with our mtype for OSS and the buf object
        if (msgreceivenw(1, buf))
        {
            if (buf->data.status == CLAIM) //child sent a max claim vector to oss
            {
                //log the max claim vector and state the claim inside of the resource manager struct
                log.logMaxClaim(shclk, buf->data);
                r.stateclaim(buf->data.pid, buf->data.resarray);

                msgsend(1, buf->data.pid+2);
            }
            else if (buf->data.status == REQ) //child has sent a request for a resource to oss
            {
                //increment the number of requests (to print out the resource table once we reach 20)
                reqs++;

                //set a local variable of the allocated value
                int allocated = r.allocate(buf->data.pid, buf->data.resi, buf->data.resamount); //allocate function will run the deadlock avoidance algorithm


                if (allocated == 0)//if 0 is returned then the request can be granted to the child process
                {
                    msgsend(1, buf->data.pid+2);

                    log.logReqGranted(
                        shclk, buf->data,
                        r.desc[buf->data.resi].shareable,
                        r.lastBlockTest);

                }
                else if (allocated == 1)// if 1 is returned the request was higher than the max the sytem has the process said it would need
                {
                    //block the process that made the too high of a request
                    blockedRequests.push_back(buf->data);
                    log.logReqDenied(shclk, buf->data, r.desc[buf->data.resi].shareable);
                }
                else if (allocated == 2)// if 2 is returned a possible deadlock could occur if the request was granted
                {
                    //block the process since it cannot run without the new resource
                    blockedRequests.push_back(buf->data);
                    log.logReqDeadlock(
                        shclk, buf->data,
                        r.desc[buf->data.resi].shareable,
                        r.lastBlockTest);
                }
                if (reqs % 20 == 19)
                {
                    log.logAlloc(r.desc, r.sysmax);
                }
            }
            else if (buf->data.status == REL) //a message was sent that the process released resources back into the system
            {
                //.release will then increment the resource that was given back to the entire systm at that time
                r.release(buf->data.pid, buf->data.resi, buf->data.resamount);

                msgsend(1, buf->data.pid+2);
                log.logRel(shclk, buf->data, blockedRequests.size());

                //check to see if we can unblock any processes if they needed that resource and now we possibly have enough
                unblockAfterRelease(shclk, r, blockedRequests, log);
            }
            else if (buf->data.status == TERM)// a message was sent that the process has now terminated
            {
                //loop through all of the resources that the process was using and release them back into the system
                for (int i : range(20))
                {
                    r.release(buf->data.pid, i, r.desc[i].alloc[buf->data.pid]); //.release will handle putting resources back into the system
                }

                //wait for the child to fully terminate
                waitforchildpid(buf->data.realpid, conc_count);

                //unset the pid inside of the bitmap to free up a slot
                r.unsetpid(buf->data.pid);

                log.logTerm(shclk, buf->data, blockedRequests.size());
                unblockAfterRelease(shclk, r, blockedRequests, log); //check to see if any processes can be unblocked since we released multiple resources into the system

            }
        }

        //increment the clock by 200 microseconds
        shclk->inc(2e5);
    }

    std::cout << log.logExit(shclk, quittype, max_count) << "\n";
    std::cout << "For simulation details, please see " << log.logfile.name << "\n";
    cleanup(shclk, r, conc_count); //cleanup any memory that is still remaining

}

int main(int argc, char** argv)
{

    signal(SIGINT, signalhandler);
    signal(SIGALRM, signalhandler);

    std::string runpath, pref;
    parserunpath(argv, runpath, pref);
    setupprefix(pref);
    
    if (!pathdepcheck(runpath, "user"))
    {
      pathdeperror();
    }

    std::vector<std::string> opts{};
    bool flags[2] = {false, false};
    int optind = getcliarg(argc, argv, "", "hv", opts, flags);

    Log log = Log(runpath + "output-" + epochstr() + ".log", 100000, flags[1]); //creates the log object to log into a file
    int max_time = 5;

    testopts(argc, argv, pref, optind, flags); //checks to see if, any, flags were sent into the program call

    alarm(max_time); //kill timer
    main_loop(runpath, log);

    return 0;
}
