#ifndef CLOCK_WORK_H
#define CLOCK_WORK_H

#include <string>

struct clk
{
    long clk_s;
    long clk_n;

    clk()
    {
        clk_s = 0;
        clk_n = 0;
    }

    float tofloat();
    std::string tostring();
    long tonano();
    void set(float time);
    void set(std::string time);
    void inc(long ns);
    void dec(long ns);
    float nextrand(long maxns);
};

std::string ClockPadding(clk* shclk);

#endif
