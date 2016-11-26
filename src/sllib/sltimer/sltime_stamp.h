/********************************************************************
	created:	2016/08/04
	created:	4:8:2016   22:34
	filename: 	c:\Users\ddc\Desktop\shyloo\libs\common\sltimestamp.h
	file path:	c:\Users\ddc\Desktop\shyloo\libs\common
	file base:	sltimestamp
	file ext:	h
	author:		ddc
	
	purpose:	
*********************************************************************/

#ifndef _SL_TIMESTAMP_H_
#define _SL_TIMESTAMP_H_
#include "slplatform.h"
#include "sltype.h"
namespace sl
{
namespace timer
{
enum SLTimingMethod
{
	RDTSC_TIMING_METHOD,		///< 自cpu上电以来所经过的时钟周期数，达到纳秒级的计时精度
	GET_TIME_OF_DAY_TIMING_METHOD,
	GET_TIME_TIMING_METHOD,
	NO_TIMING_METHOD,
};

extern SLTimingMethod g_timingMethod;

const char* getTimingMethodName();

#ifdef SL_OS_WINDOWS
#ifdef SL_USE_RDTSC
#pragma warning (push)
#pragma warning (disable: 4035)
inline uint64 timestamp()
{
	__asm rdstc
}
#pragma warning (pop)
#else
#include <Windows.h>
inline uint64 timestamp()
{
	LARGE_INTEGER	counter;
	QueryPerformanceCounter( &counter);
	return counter.QuadPart;
}
#endif
#endif

#ifdef SL_OS_LINUX
inline uint64 timestamp_rdtsc()
{
	uint32 rethi, retlo;
	__asm__ __volatile__(
		"rdtsc":
		"=d"	(rethi),
		"=a"	(retlo)
		);
	return uint64(rethi) << 32 | retlo;
}

#include <sys/time.h>
inline uint64 timestamp_gettimeofday()
{
	timespec tv;
	gettimeofday(&tv, NULL);
	return 1000000ULL * uint64(tv.tv_sec) + uint64(tv.tv_usec);

}

#include <time.h>
#include <asm/unistd.h>

inline uint64 timestamp_gettime()
{
	timespec tv;
	assert(syscall(__NR_clock_gettime, CLOCK_MONOTONIC, &tv) == 0)
	return 1000000000ULL * tv.tv_sec + tv.tv_nsec;
}

inline uint64 timestamp()
{
#ifdef SL_USE_RDTSC
	return timestamp_rdtsc();
#else
	if(g_timingMethod == RDTSC_TIMING_METHOD)
		return timestamp_rdtsc();
	else if(g_timingMethod == GET_TIME_OF_DAY_TIMING_METHOD)
		return timestamp_gettimeofday();
	else
		return timestamp_gettime();
#endif
}
#endif

uint64 stampsPerSecond();
double stampsPerSecondD();

uint64 stampsPerSecond_rdtsc();
double stampsPerSecondD_rdtsc();

uint64 stampsPerSecond_gettimeofday();
double stampsPerSecondD_gettimeofday();

inline double stampsToSeconds(uint64 stamps)
{
	return double(stamps)/stampsPerSecondD();
}

class TimeStamp
{
public:
	TimeStamp(uint64 stamps = 0):m_llStamp(stamps){}
	operator uint64 &() {return m_llStamp;}
	operator uint64() const {return m_llStamp;}

	inline uint64 stamp() {return m_llStamp;}

	inline uint64 InSeconds() const;
	inline uint64 InMilliSeconds() const;
	inline uint64 InNanoSeconds() const;
	inline void SetInSeconds(uint64 seconds);
	inline void SetInMilliSeconds(uint64 milliSeconds);
	inline void SetInNanoSeconds(uint64 nanoSeconds);

	inline TimeStamp ageInStamps() const;
	inline uint64 ageInSeconds() const;
	inline uint64 ageInMilliSeconds() const;
	inline uint64 ageNanoSeconds() const;

	inline static uint64 toSeconds(uint64 stamps);
	inline static uint64 toMilliSeconds(uint64 stamps);
	inline static uint64 toNanoSeconds(uint64 stamps);

	inline static TimeStamp fromSeconds(uint64 seconds);
	inline static TimeStamp fromMilliSeconds(uint64 milliSeconds);
	inline static TimeStamp fromNanoSeconds(uint64 nanoSeconds);

public:
	uint64			m_llStamp;
};

inline uint64 TimeStamp::toSeconds(uint64 stamps)
{
	return (uint64)(stamps/stampsPerSecondD());
}

inline uint64 TimeStamp::toMilliSeconds(uint64 stamps)
{
	return (uint64)(((double)(stamps/stampsPerSecondD())) * 1000); 
}

inline uint64 TimeStamp::toNanoSeconds(uint64 stamps)
{
	return (uint64)(((double)(stamps/stampsPerSecondD())) * 1000000000);
}

inline TimeStamp TimeStamp::fromSeconds(uint64 seconds)
{
	return uint64(seconds * stampsPerSecondD());
}

inline TimeStamp TimeStamp::fromMilliSeconds(uint64 milliSeconds)
{
	return uint64((double)(milliSeconds/1000) * stampsPerSecondD());
}

inline TimeStamp TimeStamp::fromNanoSeconds(uint64 nanoSeconds)
{
	return uint64((double)(nanoSeconds/1000000000) * stampsPerSecondD());
}

inline uint64 TimeStamp::InSeconds() const
{
	return toSeconds(m_llStamp);
}

inline uint64 TimeStamp::InMilliSeconds() const
{
	return toMilliSeconds(m_llStamp);
}

inline uint64 TimeStamp::InNanoSeconds() const
{
	return toNanoSeconds(m_llStamp);
}

inline void TimeStamp::SetInSeconds(uint64 seconds)
{
	m_llStamp = fromSeconds(seconds);
}

inline void TimeStamp::SetInMilliSeconds(uint64 milliSeconds)
{
	m_llStamp = fromMilliSeconds(milliSeconds);
}

inline void TimeStamp::SetInNanoSeconds(uint64 nanoSeconds)
{
	m_llStamp = fromNanoSeconds(nanoSeconds);
}

inline TimeStamp TimeStamp::ageInStamps() const
{
	return timestamp() - m_llStamp;
}

inline uint64 TimeStamp::ageInSeconds() const
{
	return toSeconds(this->ageInStamps());
}

inline uint64 TimeStamp::ageInMilliSeconds() const
{
	return toMilliSeconds(this->ageInStamps());
}

inline uint64 TimeStamp::ageNanoSeconds() const
{
	return toNanoSeconds(this->ageInStamps());
}
}
}// namespace sl
#endif