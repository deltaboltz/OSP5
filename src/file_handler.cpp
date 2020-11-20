/* Author:      Connor Schultz
 * Created:     October 6, 2020
 * Last edit:   October 31, 2020
 */

#include <fstream>
#include <string>

//includes of custom header files
#include "error_handler.h"
#include "file_handler.h"

int File::readline(std::string& outstr) //reads in a line from an fstream to put into file object
{

    std::ifstream stream(this->name.c_str(), this->mode);

    if (std::getline(stream, outstr)) return 1;

    if (stream.bad()) customerrorquit("Unable to read from file");
    return 0;
}

void File::writeline(std::string line) //inserts a new line into the file object
{
    this->write(line + "\n");
}

void File::write(std::string msg)
{
    std::ofstream stream(this->name.c_str(), this->mode);

    stream << msg;
    if (stream.bad()) customerrorquit("Unable to write to file");


    stream.flush();
}
