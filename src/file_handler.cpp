/* Author:      Connor Schultz
 * Created:     October 6, 2020
 * Last edit:   October 31, 2020
 */

#include <fstream>
#include <string>
#include "error_handler.h"
#include "file_handler.h"

int File::readline(std::string& outstr) {

    if (std::getline(*this->stream, outstr))    return 1;

    if (this->stream->bad()) customerrorquit("Unable to read from file");
    return 0;
}

void File::writeline(std::string line) {
    this->write(line + "\n");
}

void File::write(std::string msg)
{

    this->stream = new std::fstream(this->name.c_str(), this->mode);

    (*this->stream) << msg;

    if (this->stream->bad()) customerrorquit("Unable to write to file");

    this->stream->flush();
    delete this->stream;
}
