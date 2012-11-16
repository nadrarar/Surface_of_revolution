#include "high_resolution_timer.h"

HighResolutionTime::HighResolutionTime()
{
#ifdef	WIN32
	QueryPerformanceFrequency(&this->ticksPerSecond);
	QueryPerformanceCounter(&this->initializationTicks);
	previousTicks = initializationTicks;
#endif
}

double HighResolutionTime::TimeSinceLastCall()
{
	double result;
#ifdef	WIN32
	LARGE_INTEGER now;
	LARGE_INTEGER t;

	QueryPerformanceCounter(&now);
	t.QuadPart = now.QuadPart - previousTicks.QuadPart;
	result = ((double) t.QuadPart) / ((double) ticksPerSecond.QuadPart);
	previousTicks = now;
#endif
	return result;
}

double HighResolutionTime::TotalTime()
{
	double result;
#ifdef	WIN32
	LARGE_INTEGER now;
	LARGE_INTEGER t;

	QueryPerformanceCounter(&now);
	t.QuadPart = now.QuadPart - initializationTicks.QuadPart;
	result = ((double) t.QuadPart) / ((double) ticksPerSecond.QuadPart);
#endif
	return result;
}
