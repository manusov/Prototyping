/*
OpenGL GPUstress.
Benchmarks timer functions class.
*/

#include "Timer.h"

Timer::Timer() : status(FALSE), tscFrequency(0.0), tscPeriod(0.0),
	             fpc{ 0 }, ftsc{ 0 }, latchApplication{ 0 }, latchPerformance{ 0 },
	             latchAntiBlinkFPS { 0 }, latchAntiBlinkMBPS{ 0 },
	             latchCurrentFPS{ 0 }, latchCurrentMBPS{ 0 },
	             busTrafficTotalTime{0}, framesCount(0), bytesCountTotal(0)
{
	int regs[4]{ 0 };
	__cpuid(regs, 0);
	if (regs[0] > 0)
	{
		__cpuid(regs, 1);
		if ((regs[3] & 0x10) && (regs[3] & 0x2000000))  // CPUID function 1 register EDX bit 4 = TSC, bit 25 = SSE.
		{
			status = precisionMeasure(fpc, ftsc);
			if (status)
			{
				tscPeriod = 1.0 / static_cast<double>(ftsc.QuadPart);
				tscFrequency = 1.0 / tscPeriod;
			}
		}
	}
}
Timer::~Timer()
{

}
BOOL Timer::getStatus()
{
	return status;
}
double Timer::getTscFrequency()
{
	return tscFrequency;
}
double Timer::getTscPeriod()
{
	return tscPeriod;
}
void Timer::resetStatistics()
{
	latchPerformance.QuadPart = __rdtsc();
	latchAntiBlinkFPS.QuadPart = __rdtsc();
	latchAntiBlinkMBPS.QuadPart = __rdtsc();
	latchCurrentFPS.QuadPart = 0;
	latchCurrentMBPS.QuadPart = 0;
	busTrafficTotalTime.QuadPart = 0;
	framesCount = 0;
	bytesCountTotal = 0;
}
void Timer::startApplicationSeconds()
{
	latchApplication.QuadPart = __rdtsc();
}
double Timer::getApplicationSeconds()
{
	return (__rdtsc() - latchApplication.QuadPart) * tscPeriod;
}
void Timer::startPerformanceSeconds()
{
	latchPerformance.QuadPart = __rdtsc();
}
double Timer::getPerformanceSeconds()
{
	return (__rdtsc() - latchPerformance.QuadPart) * tscPeriod;
}
void Timer::startFrameSeconds()
{
	latchCurrentFPS.QuadPart = __rdtsc();
	framesCount++;
}
double Timer::stopFrameSeconds()
{
	return 1.0 / ((__rdtsc() - latchCurrentFPS.QuadPart) * tscPeriod);
}
void Timer::startTransferSeconds()
{
	latchCurrentMBPS.QuadPart = __rdtsc();
}
double Timer::stopTransferSeconds(DWORD64 addend)
{
	bytesCountTotal += addend;
	DWORD64 currentTransferTime = __rdtsc() - latchCurrentMBPS.QuadPart;
	busTrafficTotalTime.QuadPart += currentTransferTime;
	return addend / 1048576.0 / (currentTransferTime * tscPeriod);
}
double Timer::getTransferSeconds()
{
	return busTrafficTotalTime.QuadPart * tscPeriod;
}
double Timer::getAverageFPS()
{
	return framesCount / ((__rdtsc() - latchPerformance.QuadPart) * tscPeriod);
}
double Timer::getAverageMBPS()
{
	double megabytesTotal = bytesCountTotal / 1048576.0;
	return megabytesTotal / (busTrafficTotalTime.QuadPart * tscPeriod);
}
DWORD64 Timer::getFramesCount()
{
	return framesCount;
}
double Timer::getMegabytesCount()
{
	return bytesCountTotal / 1048576.0;
}
BOOL Timer::antiBlinkFPS()
{
	BOOL enable = FALSE;
	LARGE_INTEGER t;
	t.QuadPart= __rdtsc();
	double dt = ((t.QuadPart - latchAntiBlinkFPS.QuadPart) * tscPeriod);
	if (dt > 0.5)
	{
		latchAntiBlinkFPS.QuadPart = t.QuadPart;
		enable = TRUE;
	}
	return enable;
}
BOOL Timer::antiBlinkMBPS()
{
	BOOL enable = FALSE;
	LARGE_INTEGER t;
	t.QuadPart = __rdtsc();
	double dt = ((t.QuadPart - latchAntiBlinkMBPS.QuadPart) * tscPeriod);
	if (dt > 0.5)
	{
		latchAntiBlinkMBPS.QuadPart = t.QuadPart;
		enable = TRUE;
	}
	return enable;
}
BOOL Timer::precisionMeasure(LARGE_INTEGER& hzPc, LARGE_INTEGER& hzTsc)
{
	BOOL status = FALSE;
	LARGE_INTEGER c1;
	LARGE_INTEGER c2;
	if (QueryPerformanceFrequency(&hzPc))  // Get reference frequency.
	{
		if (QueryPerformanceCounter(&c1))
		{
			c2.QuadPart = c1.QuadPart;
			while (c1.QuadPart == c2.QuadPart)
			{
				status = QueryPerformanceCounter(&c2);  // Wait for first timer change, for synchronization.
				if (!status) break;
			}
			if (status)
			{
				hzTsc.QuadPart = __rdtsc();
				c1.QuadPart = c2.QuadPart + hzPc.QuadPart;
				while (c2.QuadPart < c1.QuadPart)
				{
					status = QueryPerformanceCounter(&c2);  // Wait for increments count per 1 second.
					if (!status) break;
				}
				hzTsc.QuadPart = __rdtsc() - hzTsc.QuadPart;
			}
		}
	}
	return status;
}


