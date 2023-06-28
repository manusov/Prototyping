/*
OpenGL GPUstress.
Benchmarks timer functions class header.
*/

#pragma once
#ifndef TIMER_H
#define TIMER_H

#include <windows.h>
#include <intrin.h>
#include "Global.h"

class Timer
{
public:
    Timer();
    ~Timer();
    BOOL getStatus();
    double getTscFrequency();
    double getTscPeriod();
    void latchSeconds1();
    double getSeconds1();
    void latchSeconds2();
    double getSeconds2();
    void latchSeconds3();
    double getSeconds3();
private:
    BOOL precisionMeasure(LARGE_INTEGER& hzPc, LARGE_INTEGER& hzTsc);
    LARGE_INTEGER fpc;
    LARGE_INTEGER ftsc;
    LARGE_INTEGER latch1;
    LARGE_INTEGER latch2;
    LARGE_INTEGER latch3;
    BOOL status;
    double tscFrequency;
    double tscPeriod;
};

#endif // TIMER_H


