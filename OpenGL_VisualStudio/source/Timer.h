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
    void resetStatistics();
    void startApplicationSeconds();
    double getApplicationSeconds();
    void startPerformanceSeconds();
    double getPerformanceSeconds();
    void startFrameSeconds();
    double stopFrameSeconds();
    void startTransferSeconds();
    double stopTransferSeconds(DWORD64 addend);
    double getTransferSeconds();
    double getAverageFPS();
    double getAverageMBPS();
    DWORD64 getFramesCount();
    double getMegabytesCount();
    BOOL antiBlinkFPS();
    BOOL antiBlinkMBPS();
private:
    BOOL precisionMeasure(LARGE_INTEGER& hzPc, LARGE_INTEGER& hzTsc);
    BOOL status;
    double tscFrequency;
    double tscPeriod;
    LARGE_INTEGER fpc;
    LARGE_INTEGER ftsc;
    LARGE_INTEGER latchApplication;
    LARGE_INTEGER latchPerformance;
    LARGE_INTEGER latchAntiBlinkFPS;
    LARGE_INTEGER latchAntiBlinkMBPS;
    LARGE_INTEGER latchCurrentFPS;
    LARGE_INTEGER latchCurrentMBPS;
    LARGE_INTEGER busTrafficTotalTime;
    DWORD64 framesCount;
    DWORD64 bytesCountTotal;
};

#endif // TIMER_H


