//
// Created by connor on 10/26/2020.
//

#ifndef CHILDHANDLER_H
#define CHILDHANDLER_H

#include <string>

char** makeargv(std::string line, int& sizeout);
void freeargv(char** argv, int size);
int forkexec(std::string cmd, int& procCounter);
int forkexec(const char* cmd, int& procCounter);
int updatechildcount(int& procCount);
int waitforanychild(int& procCount);
int waitchildpid(int pid, int& procCount);

void killall();
#endif
