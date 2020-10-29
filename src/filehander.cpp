//
// Created by connor on 10/27/2020.
//

#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include "errors.h"
#include "filehandler.h"

std::vector<std::shared_ptr<std::ofstream>> outfiles;
std::vector<std::shared_ptr<std::ifstream>> infiles;

int addout(const char* name)
{
    std::shared_ptr<std::ofstream> out(new(std::ofstream));
    out->open(name);

    if(!out->is_open())
    {
        perrorquit();
    }
    outfiles.push_back(out);
    return outfiles.size() - 1;
}

int addout_append(const char* name)
{
    std::shared_ptr<std::ofstream> out(new(std::ofstream));
    out->open(name, std::ios_base::app);

    if(!out->is_open())
    {
        perrorquit();
    }

    outfiles.push_back(out);
    return outfiles.size();
}

void writeline(int filenum, std::string line)
{
    (*outfiles[filenum]) << line << "\n";

    if(outfiles[filenum]->bad())
    {
        perrorquit();
    }

    outfiles[filenum]->flush();
}
