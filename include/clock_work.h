//
// Created by Connor on 10/26/2020.
//

#ifndef CLOCK_WORK_H
#define CLOCK_WORK_H

#include <stdlib.h>
#include <string>

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
    std::string toString();

    void increment(int nanoSeconds);
    void decrement(int nanoSeconds);

    float nextrand(int maxNano);

    void set(float time);
    void set(std::string time);
};



#endif
