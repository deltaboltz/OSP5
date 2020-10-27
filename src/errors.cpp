//
// Created by connor on 10/27/2020.
//

#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include "sharedmemory.h"
#include "errors.h"

std::string rawprefix;
std::string prefix;

void setupprefix(const char* arg0)
{
    prefix = arg0;
    rawprefix = arg0;
    prefix += ": Error";
}

void perrorquit()
{
    perror(prefix.c_str());
    ipc_cleanup();
    exit(1);
}

void helpprompt(const char* error)
{
    std::cerr << prefix << ": " << error << "\n";
    std::cerr << "Please use ' " << rawprefix;
    std::cerr << " -h for help\n";
    ipc_cleanup();
    exit(1);
}