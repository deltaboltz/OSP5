/* Author:      Connor Schultz
 * Created:     September 9, 2020
 * Last edit:   October 28, 2020
 */

#include <unistd.h>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <sys/stat.h>

//includes of custom header files
#include "error_handler.h"
#include "util.h"
#include "cmd_handler.h"

char* getoptstr(const char* options, const char* flags) //makes a string for the getopt of cmd flags
{
    int optlen = strlen(options);
    int flglen = strlen(flags);
    char* optstr = new char[optlen * 2 + flglen + 1];

    //getopt returns : if the required argument has not been met by the program call
    optstr[0] = ':';
    int i = 1;

    //loop through to find the next required getopt flag
    for (int j : range(optlen))
    {
        optstr[i++] = options[j];
        optstr[i++] = ':';
    }

    for (int j : range(flglen))
    {
        optstr[i++] = flags[j];
    }
    return optstr;
}

int getcliarg(int argc, char** argv, const char* options, const char* flags, std::vector<std::string> &optout, bool* flagout)
{

    char* optstr = getoptstr(options, flags);
    int c;
    opterr = 0;

    //loop through argv for the flags then stores the value
    while ((c = getopt(argc, argv, optstr)) != -1)
    {
        if (c == '?') //if we have an unknown flag then tell the program to stop
        {
            custerrhelpprompt("Unknown argument '-" + std::string(1, (char)optopt) +    "'");
        }
        if (c == ':') //if a required flag is not met then tell the program to stop
        {
            custerrhelpprompt("Option '-" + std::string(1, (char)optopt) + "' requires an argument!");
        }

        int optindex = -1;

        //loop through to see if any matched flag is an option needed for the program
        for (int j : range((int)strlen(options)))
        {
            if (c == options[j])
            {
                optindex = j;

                optout[optindex] = std::string(optarg);
                break;
            }
        }
        if (optindex == -1)
        {
            for (int j : range((int)strlen(flags)))
            {
                if (c == flags[j])
                {
                    flagout[j] = true;
                    break;
                }
            }
        }
    }

    return optind; //returns how my arguments were given in the program call
}

void parserunpath(char** argv, std::string& runpath, std::string& pref) //checks to see if argv[0] to see if need to put the runpath on the programm call
{


    std::string rawpath = argv[0];
    size_t split = rawpath.rfind('/')+1;

    if (split != 0)
    {
        //adds the runpath to the program call if not given by the initial call
        runpath = rawpath.substr(0, split);
        pref = rawpath.substr(split);
    }
    else
    {
        runpath = std::string("./");
        pref = std::string(argv[0]);
    }
}

bool pathdepcheck(std::string runpath, std::string depname) //check to makes sure the needed binaries of the programs are in the path to run
{
    struct stat buffer;
    std::string depcheck = runpath + depname;
    if (stat(depcheck.c_str(), &buffer) == 0) return true;
    return false;
}
