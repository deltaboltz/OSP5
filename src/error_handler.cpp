/* Author:      Connor Schultz
 * Created:     September 10, 2020
 * Last edit:   October 28, 2020
 */

#include <string>
#include <iostream>

//include of custom header files
#include "sharedmemory.h"
#include "error_handler.h"

std::string rawprefix; //set to store only argv[0]
std::string prefix; //set to store any error in argv[0]

void setupprefix(std::string arg0) //sets the prefix for the program to display the correct error message
{
    prefix = arg0;
    rawprefix = arg0;
    prefix += ": Error";
}

void perrandquit() //prints the detailed perror message and cleans up any possible memory that was made
{
    perror(prefix.c_str());
    ipc_cleanup();
    exit(-1);
}

void customerrorquit(const char* error) //sends a custom error to the console before termination. cleans up memroy that was made
{
    std::cerr << prefix << ": " << error << "\n";
    ipc_cleanup();
    exit(-1);
}

void customerrorquit(std::string error) //overloaded function incase the error was sent in as a string
{

    customerrorquit(error.c_str());
}

void custerrhelpprompt(const char* error) //If an error occured at the start of the program, the help message will appear to help the user
{
    std::cerr << prefix << ": " << error << "\n";
    std::cerr << "Please run '" << rawprefix;
    std::cerr << " -h' for more assistance!\n\n";
    ipc_cleanup(); //cleans up memory if any was made at runtime
    exit(-1);
}

void custerrhelpprompt(std::string error) //overloaded function incase the prompt was sent in as a string
{

    custerrhelpprompt(error.c_str());
}

void pathdeperror() //Displays a message if the program was not run with the proper binaries
{
    customerrorquit(
        std::string("One or more dependentent binaries is ") +\
        std::string("missing, please ensure that you are ") +\
        std::string("running the program from a valid path!\n"));
}
