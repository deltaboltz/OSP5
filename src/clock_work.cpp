/* Author:      Connor Schultz
 * Date:        November 3, 2020
 * Last Edit:   November 5, 2020
 */

#include <ctime>
#include <cstring>

//includes of custom header files
#include "util.h"
#include "clock_work.h"

float clk::tofloat()
{
    //returns the clock with nano seconds converted into decimal value seconds
    return this->clk_s + (float)this->clk_n/(float)1e9;
}

std::string clk::tostring()
{
    //sprintf log buffer
    char buf[256];

    //sprintf the time of the clock in S.N
    sprintf(buf, "%ld.%09ld", this->clk_s, this->clk_n);

    //return the buffer of the clock printout
    return std::string(buf);
}

void clk::set(float time)
{
    //set the clock to the called time of the System
    //seconds and nanoseconds set respectively
    this->clk_s = (long)time;
    this->clk_n = (long)((time - this->clk_s) * 1e9);
}

void clk::set(std::string time)
{
    float ftime = std::stof(time);
    this->set(ftime);
}

void clk::inc(long ns) //increments the clock by the time sent in from the system
{
    //sets the nanoseconds in clock to the sent in nanoseconds by the system
    this->clk_n += ns;

    //loop through to increment the clock by one second if the time of
    //nanoseconds reaches 1 second logically
    //then increment the seconds time by 1
    while (this->clk_n >= 1e9)
    {
        this->clk_n -= 1e9;
        this->clk_s += 1;
    }
}

void clk::dec(long ns) //decrement the clock by the time given in nanoseconds
{
    //set the nanoseconds time in the clock struct to that minus the nanoseconds given
    this->clk_n -= ns;

    //loop through the time if nanoseconds is then less than 0 add a "second" to
    //nanoseconds and decrement the seconds
    while (this->clk_n < 0)
    {
        this->clk_n += 1e9;
        this->clk_s -= 1;
    }

    //if both the seconds time is zero we need to reset the clock back to 0.0
    if (this->clk_s < 0)
    {
        this->clk_n = 0;
        this->clk_s = 0;
    }
}

float clk::nextrand(long maxns) //sends the random time given in by the function call to increment the clock in that sent time
{
    //a placeholder clock object to hold a placeholder seconds and nanoseconds
    clk copy;
    copy.clk_s = this->clk_s;
    copy.clk_n = this->clk_n;

    //increment the clock by the maxnanoseconds sent in
    copy.inc(rand() % maxns);
    return copy.tofloat();
}

long clk::tonano() //converts the time into all nano time e.g. 1e9 values
{
    return this->clk_s*1e9 + this->clk_n;
}

std::string ClockPadding(clk* shclk) //pads the clock in order to print more readable
{
    std::string out = "  ";
    for (int i : range(shclk->tostring().size()))
    {
      out += " ";
    }
    return out;
}
