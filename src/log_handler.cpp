/*  Author:     Connor Schultz
 *  Created:    November 17, 2020
 *  Last Edit:  November 17, 2020
 */

#include <csignal>

//include of custom header files
#include "file_handler.h"
#include "resource_handler.h"
#include "util.h"
#include "log_handler.h"

void Log::logline(std::string msg, bool force) //writes a line to the log file object
{
    //if force is true or the linecount has not reached the maximum allowable lines
    //then we will write to the file. Else we will not write to the file object anymore
    if (force || this->linecount < this->maxlinecount)
    {
        this->logfile.writeline(msg);
        this->linecount++;
    }
}

void Log::logNewPID(clk* shclk, int pid, int max_count) //Logs a new pid ONLY if verbose is on
{
    if (!this->verbose) return;

    //writes the new pid created into the log file
    this->logline(shclk->tostring() + ": Created PID " + std::to_string(pid));
}

void Log::logMaxClaim(clk* shclk, Data d) //logs the maxclaim sent in from the newly created child process ONLY if verbose in on
{
    if (!this->verbose) return;

    std::string out = shclk->tostring() + ": PID " + std::to_string(d.pid) +   " Max Claim: [";

    //loop through to get the values of each maxclaim resource the proccess says it will ever need
    for (int i : range(19))
    {
        out += std::to_string(d.resarray[i]) + ", ";
    }

    out += std::to_string(d.resarray[19]) + "]";
    this->logline(out);
}

std::string logReqBaseMsg(clk* shclk, Data d, bool shareable, bool printclk) //logs the requested resource of the child process works WITH verbose off
{
    std::string out = printclk ? shclk->tostring() + ": ":ClockPadding(shclk); //get the clock padding from clock_works.h

    return out + "PID " + std::to_string(d.pid) + " requested " + std::to_string(d.resamount) + " of " + (shareable ? "shareable resource " : "") +
        "R" + std::to_string(d.resi) + " and was ";
}

void Log::logDeadlockTest(clk* shclk, bool issafe, std::vector<int>& blocked) //logs if a process could've deadlocked the program works WITH verbose off
{
    std::string out = ClockPadding(shclk);

    this->logline(shclk->tostring() + ": Deadlock avoidance algorithm ran");
    this->logline(out + (issafe ? "Safe":"Unsafe") + " state after granting request");

    //loop through to find which processes were found to be unsafe in the program to print out those possiblilities
    if (!issafe)
    {
        if (blocked.size()-1 > 0)
        {
            for (int PID : range(blocked.size()-1))
            {
                out += "P" + std::to_string(PID) + ", ";
            }
        }

        out += "P" + std::to_string(blocked.back()) + " could deadlock";
        this->logline(out);
    }
}

void Log::logReqGranted(clk* shclk, Data d, bool shareable, std::vector<int>& blocked) //log whether a request was granted work ONLY with verbose on
{
    if (!this->verbose) return;
    this->logDeadlockTest(shclk, true, blocked);
    this->logline(logReqBaseMsg(shclk, d, shareable, false) + "granted");
}

void Log::logReqDenied(clk* shclk, Data d, bool shareable) //log whether a request was denied works WITH verbose off
{
    this->logline(logReqBaseMsg(shclk, d, shareable, true) + "denied due to lack of availability");
}

void Log::logReqDeadlock(clk* shclk, Data d, bool shareable, std::vector<int>& blocked) //log if a possible deadlock may have occured works WITH verbose off
{
    this->logDeadlockTest(shclk, false, blocked);

    this->logline(logReqBaseMsg(shclk, d, shareable, false) + "denied due to possible deadlock");
}

void Log::logUnblockCheck(clk* shclk, Data d, int blockSize) //log when checking to see if any processes can now be unblocked due to resources being released
{
    if (blockSize)
    {
        this->logline(shclk->tostring() + ": Checking if any processes can be " + "unblocked (Currently " + std::to_string(blockSize) + " blocked)");
    }
}

void Log::logRel(clk* shclk, Data d, int blockSize) //log when a process released resources back into the system, works ONLY with verbose on
{
    if (!this->verbose) return;

    this->logline(shclk->tostring() + ": PID " + std::to_string(d.pid) + " released " + std::to_string(d.resamount) + " of R" + std::to_string(d.resi));

    this->logUnblockCheck(shclk, d, blockSize);
}

void Log::logUnblock(clk* shclk, Data d) //log when a process did become unblocked works WITH verbose off
{
    this->logline(shclk->tostring() + ": PID " + std::to_string(d.pid) +
        " unblocked and request for " + std::to_string(d.resamount) + " of R" +
        std::to_string(d.resi) + " was granted");
}

void Log::logTerm(clk* shclk, Data d, int blockSize) //log when a process has terminated with ONLY when verbose in on
{
    if (!this->verbose) return;

    this->logline(shclk->tostring() + ": PID " + std::to_string(d.pid) + " terminated and released all allocated resources");

    this->logUnblockCheck(shclk, d, blockSize);
}

std::string Log::logExit(clk* shclk, int quittype, int max_count) //log when oss has finished with any coniditon
{
    std::string out;
    if (quittype == SIGINT) //oss was interrupted and termed
    {
        out = "oss interrupted and terminated";
    }
    else if (quittype == SIGALRM) //oss reached time limit given
    {
        out = "oss reached simulation time limit and terminated";
    }
    else if (quittype == 0) //oss finished fully
    {
        out = "oss successfully simulated " + std::to_string(max_count) +  " processes and terminated normally";
    }
    else //oss quit due to unknown circumstances
    {
        out = "oss terminated. Unknown quittype " + std::to_string(quittype);
    }

    this->logline(shclk->tostring() + ": " + out, true);

    return out + " at system time " + shclk->tostring();
}

void Log::logAlloc(Descriptor* desc, int* sysmax) //log when a resouce has been allocated to a process works ONLY with verbose on
{
    if (!this->verbose) return;

    char buf[256];
    char* pos = buf;
    this->logline("Current System Resources:");

    //prints all the resources available, max in the system, and whether they are shareable with a *
    pos += sprintf(pos, "        ");
    for (int j : range(20)) {
        pos += sprintf(pos, "R%d%c%s ", j,
                desc[j].shareable ? '*' : ' ',
                j < 10 ? " " : "");
    }

    this->logline(buf);
    pos = buf;
    memset(buf, 0, 256);


    pos += sprintf(pos, "Max     ");
    for (int j : range(20))
    {
      pos += sprintf(pos, "%-2d   ", sysmax[j]);
    }

    this->logline(buf);
    pos = buf;
    memset(buf, 0, 256);


    pos += sprintf(pos, "Avail   ");
    for (int j : range(20))
        pos += sprintf(pos, "%-2d   ", desc[j].avail);
    this->logline(buf);
    pos = buf;
    memset(buf, 0, 256);


    for (int PID : range(18))
    {
        pos += sprintf(pos, "P%-2d     ", PID);
        for (int descID : range(20))
            pos += sprintf(pos, "%-2d   ", desc[descID].alloc[PID]);
        this->logline(buf);
        pos = buf;
        memset(buf, 0, 256);
    }
}
