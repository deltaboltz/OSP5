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
#include "cmd_handler.h"         
#include "childhandler.h"
#include "error_handler.h"
#include "sharedmemory.h"
#include "help_handler.h"
#include "clock_work.h"
#include "resource_handler.h"
#include "log_handler.h"
#include "util.h"

volatile bool earlyquit = false;
volatile bool handlinginterrupt = false;
volatile int quittype = 0;

void signalhandler(int signum) {
    if (signum == SIGALRM || signum == SIGINT) {
        earlyquit = true;
        quittype = signum;
    }
}

void cleanup(clk* shclk, resman r, int& conc_count) {

    while (conc_count > 0) {
        killallchildren();
        updatechildcount(conc_count);
    }

    shmdetach(shclk);
    shmdetach(r.desc);
    shmdetach(r.sysmax);
    ipc_cleanup();
}

void testopts(int argc, char** argv, std::string pref, int optind, bool* flags) {
    if (flags[0]) {
        printhelp(pref);
        exit(0);
    }

    if (argc > optind) custerrhelpprompt(
        "Unknown argument '" + std::string(argv[optind]) + "'");
}

void unblockAfterRelease(clk* shclk, resman& r, std::list<Data>& blocked, Log log) {
    auto i = blocked.begin();
    while (i != blocked.end()) {
        if (r.allocate((*i).pid, (*i).resi, (*i).resamount) == 0) {
            msgsend(1, (*i).pid+2);
            log.logUnblock(shclk, (*i));
            blocked.erase(i++);
        } else {
            i++;
        }
    }
}

void main_loop(std::string runpath, Log log) {
    int max_count = 0;
    int conc_count = 0;
    int currID = 0;
    int spawnConst = 500000000;
    int pid;
    srand(getpid());

    clk* shclk = (clk*)shmcreate(sizeof(clk), currID);
    float nextSpawnTime = shclk->nextrand(spawnConst);
    msgcreate(currID);

    resman r(currID);
    pcbmsgbuf* buf = new pcbmsgbuf;

    std::list<Data> blockedRequests;

    while (!earlyquit) {
        if (max_count >= 40 && conc_count == 0) earlyquit = true;
        if (shclk->tofloat() >= nextSpawnTime && max_count < 40) {
            r.findandsetpid(pid);
            if (pid >= 0) {
                log.logNewPID(shclk, pid, ++max_count);
                forkexec(runpath + "user " + std::to_string(pid), conc_count);
                nextSpawnTime = shclk->nextrand(spawnConst);
            }
        }
        buf->mtype = 1;
        if (msgreceivenw(1, buf)) {
            if (buf->data.status == CLAIM) {
                log.logMaxClaim(shclk, buf->data);
                r.stateclaim(buf->data.pid, buf->data.resarray);
                msgsend(1, buf->data.pid+2);
            } else if (buf->data.status == REQ) {
                int allocated = r.allocate(
                        buf->data.pid, buf->data.resi, buf->data.resamount);
                if (allocated == 0) {
                    msgsend(1, buf->data.pid+2);
                    log.logReqGranted(shclk, buf->data);
                } else if (allocated == 1) {
                    blockedRequests.push_back(buf->data);
                    log.logReqDenied(shclk, buf->data);
                } else if (allocated == 2) {
                    blockedRequests.push_back(buf->data);
                    log.logReqDeadlock(shclk, buf->data);
                }
            } else if (buf->data.status == REL) {
                r.release(buf->data.pid, buf->data.resi, buf->data.resamount);
                msgsend(1, buf->data.pid+2);
                log.logRel(shclk, buf->data, blockedRequests.size());
                unblockAfterRelease(shclk, r, blockedRequests, log);
            } else if (buf->data.status == TERM) {
                for (int i : range(20)) {
                    r.release(buf->data.pid, i, r.desc[i].alloc[buf->data.pid]);
                }
                waitforchildpid(buf->data.realpid, conc_count);
                r.unsetpid(buf->data.pid);
                log.logTerm(shclk, buf->data, blockedRequests.size());
                unblockAfterRelease(shclk, r, blockedRequests, log);
            }
        } else {

        }
        shclk->inc(2e5);
    }

    std::cout << log.logExit(shclk, quittype, max_count) << "\n";
    std::cout << "For simulation details, please see " << log.logfile.name << "\n";
    cleanup(shclk, r, conc_count);

}

int main(int argc, char** argv) {

    signal(SIGINT, signalhandler);
    signal(SIGALRM, signalhandler);

    std::string runpath, pref;
    parserunpath(argv, runpath, pref);
    setupprefix(pref);
    if (!pathdepcheck(runpath, "user")) pathdeperror();

    std::vector<std::string> opts{};
    bool flags[2] = {false, false};
    int optind = getcliarg(argc, argv, "", "hv", opts, flags);

    Log log = Log(runpath + "output-" + epochstr() + ".log", 100000, flags[1]);
    int max_time = 5;

    testopts(argc, argv, pref, optind, flags);

    alarm(max_time);
    main_loop(runpath, log);

    return 0;
}
