#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <fstream>
#include <string>
#include "error_handler.h"

enum IOMODE
{
    READ=std::fstream::in,
    WRITE=std::fstream::out,
    APPEND=std::fstream::app
};

struct File
{
    std::string name;
    std::ios_base::openmode mode;

    File() {};

    File(std::string filepath, IOMODE open_mode)
    {
        name = filepath;
        mode = (std::ios_base::openmode)open_mode;
    }

    File(const File& old)
    {
        name = old.name;
        mode = old.mode;
    }

    int readline(std::string& outstr);
    void writeline(std::string line);
    void write(std::string msg);
};

#endif
