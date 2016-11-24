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
#ifdef SL_OS_WINDOWS
#else
#endif

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

#else
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

//使用 gettimeofday， 测试大概比RDTSC20倍-600倍
#include <sys/time.h>
inline uint64 timestamp_gettime()
{
	timespec tv;

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

namespace timer
{
class TimeStamp
{
public:
	TimeStamp(uint64 stamps = 0):m_llStamp(stamps){}
	operator uint64 &() {return m_llStamp;}
	operator uint64() const {return m_llStamp;}

	inline uint64 stamp() {return m_llStamp;}

	inline double InSeconds() const;
	inline void SetInSeconds(double seconds);

	inline TimeStamp ageInStamps() const;
	inline double ageInSeconds() const;

	inline static double toSeconds(uint64 stamps);
	inline static TimeStamp fromSeconds(double seconds);

public:
	uint64			m_llStamp;
};

inline double TimeStamp::toSeconds(uint64 stamps)
{
	return double(stamps)/stampsPerSecondD();
}

inline TimeStamp TimeStamp::fromSeconds(double seconds)
{
	return uint64(seconds * stampsPerSecondD());
}

inline double TimeStamp::InSeconds() const
{
	return toSeconds(m_llStamp);
}

inline void TimeStamp::SetInSeconds(double seconds)
{
	m_llStamp = fromSeconds(seconds);
}

inline TimeStamp TimeStamp::ageInStamps() const
{
	return timestamp() - m_llStamp;
}

inline double TimeStamp::ageInSeconds() const
{
	return toSeconds(this->ageInStamps());
}


}

}// namespace sl
#endif