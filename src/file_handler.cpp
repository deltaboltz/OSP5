/* Author:      Connor Schultz
 * Created:     October 6, 2020
 * Last edit:   October 31, 2020
 */

#include <fstream>
#include <string>
#include "error_handler.h"
#include "file_handler.h"

int File::readline(std::string& outstr)
{
  std::ifstream stream(this->name.cstr(), this->mode);

  if(std::getline(stream, outstr))
  {
    return 1;
  }

  if(stream.bad())
  {
    customerrorquit("Unable to read file");
  }
}

void File::writeline(std::string line)
{
    this->write(line + "\n");
}

void File::write(std::string msg)
{

    std::ofstream stream(this->name.c_str(), this->mode);

    stream << msg;

    if (this->stream->bad())
    {
      customerrorquit("Unable to write to file");
    }

    this->stream->flush();
}
