#pragma once

#ifdef	WIN32
#include <Windows.h>
#endif

class HighResolutionTime
{
public:
	HighResolutionTime();
	double TimeSinceLastCall();
	double TotalTime();

private:

#ifdef	WIN32
	LARGE_INTEGER initializationTicks;
	LARGE_INTEGER ticksPerSecond;
	LARGE_INTEGER previousTicks;
#endif

};
