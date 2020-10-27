//
// Created by Connor on 10/26/2020.
//

#ifndef CLOCK_WORK_H
#define CLOCK_WORK_H

#include <stdlib.h>

struct clck
{
    int clockSec;
    int clockNano;

    clck()
    {
        clockSec = 0;
        clockNano = 0;
    }

    float tofloat();
    void increment(int nanoSeconds);
    void decrement(int nanoSeconds);
    float nextrand(int maxNano);
};

float clck::tofloat()
{
    return (float)this->clockSec + (float)this->clockNano/(float)1e9;
}

void clck::increment(int nanoSeconds)
{
    this->clockNano += nanoSeconds;
    while(this->clockNano > 1e9)
    {
        this->clockNano -= 1e9;
        this->clockSec += 1;
    }
}

void clck::decrement(int nanoSeconds)
{
    this->clockNano -= nanoSeconds;
    while(this->clockNano < 0)
    {
        this->clockNano += 1e9;
        this->clockSec -= 1;
    }
    if(this->clockSec)
    {
        this->clockSec = 0;
        this->clockNano = 0;
    }
}

float clck::nextrand(int maxNano)
{
    clck val;
    val.clockSec = this->clockSec;
    val.clockNano = this->clockNano;
    val.increment(rand() & maxNano);
    return val.tofloat();
}

#endif
