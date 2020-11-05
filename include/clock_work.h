//
// Created by Connor on 10/26/2020.
//

#ifndef CLOCK_WORK_H
#define CLOCK_WORK_H

#include <stdlib.h>
#include <string>

struct clck
{
    long clockSec;
    long clockNano;

    clck()
    {
        clockSec = 0;
        clockNano = 0;
    }

    float tofloat();
    std::string toString();

    void increment(long nanoSeconds);
    void decrement(long nanoSeconds);

    float nextrand(long maxNano);

    void set(float time);
    void set(std::string time);
};

std::string epochlogid();


#endif
