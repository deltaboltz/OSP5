/* Author:      Connor Schultz
 * Created:     September 10, 2020
 * Last edit:   October 28, 2020
 */

#include <string>
#include <iostream>
#include "sharedmemory.h"
#include "error_handler.h"

std::string rawprefix;
std::string prefix;

void setupprefix(std::string arg0)
{
    //test comment
    prefix = arg0;
    rawprefix = arg0;
    prefix += ": Error";
}

void perrandquit()
{
    perror(prefix.c_str());
    ipc_cleanup();
    exit(-1);
}

void customerrorquit(const char* error)
{
    std::cerr << prefix << ": " << error << "\n";
    ipc_cleanup();
    exit(-1);
}

void customerrorquit(std::string error)
{

    customerrorquit(error.c_str());
}

void custerrhelpprompt(const char* error)
{
    std::cerr << prefix << ": " << error << "\n";
    std::cerr << "Please run '" << rawprefix;
    std::cerr << " -h' for more assistance!\n\n";
    ipc_cleanup();
    exit(-1);
}

void custerrhelpprompt(std::string error)
{

    custerrhelpprompt(error.c_str());
}

void pathdeperror()
{
    customerrorquit(
        std::string("One or more dependentent binaries is ") +\
        std::string("missing, please ensure that you are ") +\
        std::string("running the program from a valid path!\n"));
}
