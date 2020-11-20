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

//Includes for all selfmade header files
#include "error_handler.h"
#include "util.h"
#include "childhandler.h"

std::set<pid_t> PIDS;

char** makeargv(std::string line, int& size)
{
    //Read in the argument given by the program call
    //In this case this will be -v for verbose on or off
    std::istringstream iss(line);
    std::vector<std::string> argvector;

    //Loop through to extract all characters until end of line (whitespace)
    while (iss)
    {
        std::string sub;
        iss >> sub;
        argvector.push_back(sub);
    }

    //initialize the argument char array to hold the proper size of arguments
    size = argvector.size();
    char** out = new char*[size];

    //Loop through to copy the argument tokens back
    for (int i : range(size-1))
    {
        out[i] = new char[argvector[i].size()];
        strcpy(out[i], argvector[i].c_str());
    }

    //always set the last element in the array to the nullptr since
    //this will always be an empty string
    out[size-1] = nullptr;

    return out;
}

void freeargv(char** argv, int size)
{
    //Loop through to deallocate the char* array where the size is x
    for (int x : range(size))
    {
        delete[] argv[x];
    }

    //dellacote the outer array
    delete[] argv;
}

int forkexec(std::string cmd, int& pr_count) //Overloaded function of forkexec if the argument is a string
{
    return forkexec(cmd.c_str(), pr_count); //Call the char* version of the overloaded function
}

int forkexec(const char* cmd, int& pr_count) //Forks the child process, increment the counter, and return new PID
{
    int child_argc;

    //Initialize a char array from the makearv function
    char** child_argv = makeargv(cmd, child_argc);
    //Set a new pid to the fork call
    const pid_t child_pid = fork();

    switch(child_pid)
    {
        case -1: //If fork failed then we will exit the program and print a perror of fork
            perrandquit();
            return -1;

        case 0: //Fork was successfull

            //If the execvp of the child call was unsuccessful then we will
            //error and quit out using the customerror set below
            if (execvp(child_argv[0], child_argv) == -1)
            {
                customerrorquit("Unable to exec '" + std::string(child_argv[0]) + "'");
            }
            return 0;

        default: //Fork was successfull

            /*
             * insert the new PID
             * increment proc_count
             * free up the argv arguments
             * return the new child pid
             */
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

    //Switch on waitpid
    switch((pid = waitpid(-1, &wstatus, WNOHANG)))
    {
        case -1: //Waitpid failed, perror on waitpid and exit program
            perrandquit();
            return -1;

        case 0: //No children have terminated yet, no waiting though
            return 0;

        default: //Child has terminated erase the pid from PIDS, lower proc_count, return the pid of gone child
            PIDS.erase(pid);
            pr_count--;
            return pid;
    }
}

int waitforanychild(int& pr_count)
{
    int wstatus;
    pid_t pid;

    //switch on waitpid
    switch((pid = waitpid(-1, &wstatus, 0)))
    {
        case -1: //waitpid failed, perror and exit the program
            perrandquit();
            return -1;

        default: //child has terminated erase the pid from PIDS, lower proc_count, return the pid of gone child
            PIDS.erase(pid);
            pr_count--;
            return pid;
    }
}

int waitforchildpid(int pid, int& pr_count) //semi overloaded function, waits of certain pids
{

    int wstatus;
    pid_t pid_exit;

    //Switch on waitpid
    switch((pid_exit = waitpid(pid, &wstatus, 0)))
    {
        case -1: //waitpid failed perror and exit the program
            perrandquit();
            return -1;

        default: //child has terminated reomve from PIDS, lower proc_count, and return pid of gone child
            PIDS.erase(pid);
            pr_count--;
            return pid;
    }
}

void killallchildren()
{
    //Loop through PIDS to see if any child is still running
    for (int p : PIDS)
    {
      //if the kill function of the pid is not successfull perror and exit the program
      if (kill(p, SIGINT) == -1)
      {
          perrandquit();
      }
    }

}
