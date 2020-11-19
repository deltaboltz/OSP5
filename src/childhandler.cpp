/* Author:      Connor Schultz
 * Created:     September 9, 2020
 * Last edit:   October 28, 2020
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <csignal>
#include <sstream>
#include <vector>
#include <set>
#include <string>
#include <cstring>
#include <sys/stat.h>
#include "error_handler.h"
#include "util.h"
#include "childhandler.h"

std::set<pid_t> PIDS;

char** makeargv(std::string line, int& size) {



    std::istringstream iss(line);
    std::vector<std::string> argvector;
    while (iss)
    {
        std::string sub;
        iss >> sub;
        argvector.push_back(sub);
    }

    size = argvector.size();
    char** out = new char*[size];

    for (int i : range(size-1))
    {
        out[i] = new char[argvector[i].size()];
        strcpy(out[i], argvector[i].c_str());
    }


    out[size-1] = nullptr;
    return out;
}

void freeargv(char** argv, int size)
{

    for (int x : range(size))
    {

        delete[] argv[x];
    }

    delete[] argv;
}

int forkexec(std::string cmd, int& pr_count)
{

    return forkexec(cmd.c_str(), pr_count);
}

int forkexec(const char* cmd, int& pr_count)
{

    int child_argc;

    char** child_argv = makeargv(cmd, child_argc);
    const pid_t child_pid = fork();

    switch(child_pid)
    {
        case -1:

            perrandquit();
            return -1;

        case 0:

            if (execvp(child_argv[0], child_argv) == -1)
            {
                customerrorquit("Unable to exec '" +\
                        std::string(child_argv[0]) + "'");
            }
            return 0;

        default:

            PIDS.insert(child_pid);
            pr_count++;
            freeargv(child_argv, child_argc);
            return child_pid;
    }
}

int updatechildcount(int& pr_count)
{


    int wstatus;
    pid_t pid;
    switch((pid = waitpid(-1, &wstatus, WNOHANG)))
    {
        case -1:
            perrandquit();
            return -1;

        case 0:
            return 0;

        default:
            PIDS.erase(pid);
            pr_count--;
            return pid;
    }
}

int waitforanychild(int& pr_count)
{
    int wstatus;
    pid_t pid;
    switch((pid = waitpid(-1, &wstatus, 0)))
    {
        case -1:
            perrandquit();
            return -1;

        default:
            PIDS.erase(pid);
            pr_count--;
            return pid;
    }
}

int waitforchildpid(int pid, int& pr_count)
{

    int wstatus;
    pid_t pid_exit;

    switch((pid_exit = waitpid(pid, &wstatus, 0)))
    {
        case -1:
            perrandquit();
            return -1;

        default:
            PIDS.erase(pid);
            pr_count--;
            return pid;
    }
}

void killallchildren()
{

    for (int p : PIDS)
        if (kill(p, SIGINT) == -1)
            perrandquit();
}
