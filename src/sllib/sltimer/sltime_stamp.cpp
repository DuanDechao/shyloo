#include "sltime_stamp.h"

namespace sl
{
namespace timer
{

#define SL_USE_RDTSC
#ifdef SL_USE_RDTSC
SLTimingMethod g_timingMethod = RDTSC_TIMING_METHOD;
#else // SL_USE_RDTSC
const SLTimingMethod DEFAULT_TIMING_METHOD = GET_TIME_TIMING_METHOD;
SLTimingMethod g_timingMethod = NO_TIMING_METHOD;
#endif //SL_USE_RDTSC

const char* getTimingMethodName()
{
	switch(g_timingMethod)
	{
	case NO_TIMING_METHOD:
		return "none";

	case RDTSC_TIMING_METHOD:
		return "rdstc";

	case GET_TIME_OF_DAY_TIMING_METHOD:
		return "gettimeofday";

	case GET_TIME_TIMING_METHOD:
		return "gettime";

	default:
		return "UnKnown";
	}
}

#ifdef SL_OS_WINDOWS
#include <Windows.h>
#ifdef SL_USE_RDTSC
static uint64 calcStampsPerSecond()
{
	LARGE_INTEGER tvBefore, tvAfter;
	DWORD		  tvSleep = 500;
	uint64 stampBefore, stampAfter;

	Sleep(100);

	QueryPerformanceCounter(&tvBefore);
	QueryPerformanceCounter(&tvBefore);
	QueryPerformanceCounter(&tvBefore);

	stampBefore = timestamp();

	Sleep(tvSleep);

	QueryPerformanceCounter(&tvAfter);
	QueryPerformanceCounter(&tvAfter);
	QueryPerformanceCounter(&tvAfter);

	stampAfter = timestamp();

	uint64 countDelta = tvAfter.QuadPart - tvBefore.QuadPart;
	uint64 stampDelta = stampAfter - stampBefore;

	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	return (uint64)((stampDelta * uint64(frequency.QuadPart)) /countDelta);
}
#else // SL_USE_RDTSC
static uint64 calcStampsPerSecond()
{
	LARGE_INTEGER rate;
	QueryPerformanceFrequency(&rate);
	return rate.QuadPart;
}
#endif //SL_USE_RDSTC
#endif

#ifdef SL_OS_LINUX

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef SL_USE_RDTSC
static uint64 calcStampsPerSecond_rdtsc()
{
	struct timeval tvBefore, tvSleep = {0, 500000}, tvAfter;
	uint64 stampsBefore, stampsAfter;

	gettimeofday(&tvBefore, NULL);
	gettimeofday(&tvBefore, NULL);
	gettimeofday(&tvBefore, NULL);

	stampsBefore = timestamp();

	select(0, NULL, NULL, NULL, &tvSleep);
	
	gettimeofday(&tvAfter, NULL);
	gettimeofday(&tvAfter, NULL);
	gettimeofday(&tvAfter, NULL);
	
	stampsAfter = timestamp();

	uint64 microDelta = (tvAfter.tv_usec + 1000000 - tvBefore.tv_usec) % 1000000;

	uint64 stampsDelta = stampsAfter - stampsBefore;

	return (stampsDelta * 1000000ULL) / microDelta;
}
#else
static uint64 calcStampsPerSecond_gettime()
{
	return 1000000000ULL;
}
#endif

static uint64 calcStampsPerSecond_gettimeofday()
{
	return 1000000ULL;
}

static uint64 calcStampsPerSecond()
{
	static bool firstTime = true;
	if(firstTime)
	{
		firstTime = false;
	}

#ifdef SL_USE_RDTSC
	return calcStampsPerSecond_rdtsc();
#else //SL_USE_RDTSC
	if(g_timingMethod == RDTSC_TIMING_METHOD)
		return calcStampsPerSecond_rdtsc();
	else if(g_timingMethod == GET_TIME_OF_DAY_TIMING_METHOD)
		return calcStampsPerSecond_gettimeofday();
	else if(g_timingMethod == GET_TIME_TIMING_METHOD)
		return calcStampsPerSecond_gettime();
	else
	{
		char* timingMethod = getenv("SL_TIMING_METHOD");
		if(!timingMethod)
		{
			g_timingMethod = DEFAULT_TIMING_METHOD;
		}
		else if(strcmp(timingMethod, "rdtsc") == 0)
		{
			g_timingMethod = RDTSC_TIMING_METHOD;
		}
		else if(strcmp(timingMethod, "gettimeofday") == 0)
		{
			g_timingMethod = GET_TIME_OF_DAY_TIMING_METHOD;
		}
		else if(strcmp(timingMethod, "gettime") == 0)
		{
			g_timingMethod = GET_TIME_TIMING_METHOD;
		}
		else
		{
			g_timingMethod = DEFAULT_TIMING_METHOD;
		}

		return calcStampsPerSecond();
	}
#endif
}

uint64 stampsPerSecond_rdtsc()
{
	static uint64 stampsPerSecondCache = calcStampsPerSecond_rdtsc();
	return stampsPerSecondCache;
}

double stampsPerSecondD_rdtsc()
{
	static double stampsPerSecondCacheD = double(stampsPerSecond_rdtsc());
	return stampsPerSecondCacheD;
}

uint64 stampsPerSecond_gettimeofday()
{
	static uint64 stampsPerSecondCache = calcStampsPerSecond_gettimeofday();
	return stampsPerSecondCache;
}

double stampsPerSecondD_gettimeofday()
{
	static double stampsPerSecondCacheD = double(stampsPerSecond_gettimeofday());
	return stampsPerSecondCacheD;
}

#endif

// 每秒cpu所耗时间
uint64 stampsPerSecond()
{
	static uint64 _stampsPerSecondCache = calcStampsPerSecond();
	return _stampsPerSecondCache;
}

//每秒cpu所耗时间 double版本
double stampsPerSecondD()
{
	static double _stampsPerSecondCacheD = double(stampsPerSecond());
	return _stampsPerSecondCacheD;
}

}
}