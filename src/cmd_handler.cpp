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
#include "error_handler.h"
#include "util.h"
#include "cmd_handler.h"

char* getoptstr(const char* options, const char* flags) {

    int optlen = strlen(options);
    int flglen = strlen(flags);
    char* optstr = new char[optlen * 2 + flglen + 1];

    optstr[0] = ':';
    int i = 1;
    for (int j : range(optlen)) {
        optstr[i++] = options[j];
        optstr[i++] = ':';
    }
    for (int j : range(flglen)) {
        optstr[i++] = flags[j];
    }
    return optstr;
}

int getcliarg(int argc, char** argv, const char* options, \
              const char* flags, std::vector<std::string> &optout, \
              bool* flagout) {

    char* optstr = getoptstr(options, flags);
    int c;
    opterr = 0;

    while ((c = getopt(argc, argv, optstr)) != -1) {
        if (c == '?') {
            custerrhelpprompt("Unknown argument '-" +\
                    std::string(1, (char)optopt) +    "'");
        }
        if (c == ':') {
            custerrhelpprompt("Option '-" +\
                    std::string(1, (char)optopt) + "' requires an argument!");
        }

        int optindex = -1;
        for (int j : range((int)strlen(options))) {
            if (c == options[j]) {
                optindex = j;

                optout[optindex] = std::string(optarg);
                break;
            }
        }
        if (optindex == -1) {

            for (int j : range((int)strlen(flags))) {
                if (c == flags[j]) {

                    flagout[j] = true;
                    break;
                }
            }
        }
    }

    return optind;
}

void parserunpath(char** argv, std::string& runpath, std::string& pref) {

    std::string rawpath = argv[0];
    size_t split = rawpath.rfind('/')+1;
    if (split != 0) {
        runpath = rawpath.substr(0, split);
        pref = rawpath.substr(split);
    } else {
        runpath = std::string("./");
        pref = std::string(argv[0]);
    }
}

bool pathdepcheck(std::string runpath, std::string depname) {

    struct stat buffer;
    std::string depcheck = runpath + depname;
    if (stat(depcheck.c_str(), &buffer) == 0) return true;
    return false;
}
