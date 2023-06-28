/*
OpenGL GPUstress.
Benchmarks timer functions class.
*/

#include "Timer.h"

Timer::Timer() : fpc{ 0 }, ftsc{ 0 }, latch1{ 0 }, latch2{ 0 }, latch3{ 0 },
                 status(FALSE), tscFrequency(0.0), tscPeriod(0.0)
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
void Timer::latchSeconds1()
{
	 latch1.QuadPart = __rdtsc();
}
double Timer::getSeconds1()
{
	DWORD64 dTsc = __rdtsc() - latch1.QuadPart;
	double seconds = dTsc * tscPeriod;
	return seconds;
}
void Timer::latchSeconds2()
{
	latch2.QuadPart = __rdtsc();
}
double Timer::getSeconds2()
{
	DWORD64 dTsc = __rdtsc() - latch2.QuadPart;
	double seconds = dTsc * tscPeriod;
	return seconds;
}
void Timer::latchSeconds3()
{
	latch3.QuadPart = __rdtsc();
}
double Timer::getSeconds3()
{
	DWORD64 dTsc = __rdtsc() - latch3.QuadPart;
	double seconds = dTsc * tscPeriod;
	return seconds;
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


