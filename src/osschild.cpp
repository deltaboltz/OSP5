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
    int* shmid = (int*)shmlook(1);

    float stop = shclock->nextrand(1e6);

    while(shclock->tofloat() < stop);
    msgreceive(2);
    *shmid = getpid();

    shmdetach(shclock);
    shmdetach(shmid);

    return 0;
}