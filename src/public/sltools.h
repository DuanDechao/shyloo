#ifndef SL_PUBLIC_TOOLS_H
#define SL_PUBLIC_TOOLS_H
#include "slmulti_sys.h"
#ifdef SL_OS_WINDOWS
#include <shlwapi.h>
#endif // SL_OS_WINDOWS
#include <random>
#include <time.h>
namespace sl
{

#define GAME_REFRESH_HOUR 5
#define DAY_INTERVAL (24 * 60 * 60 * 1000)

#ifdef __cplusplus
extern "C"{
#endif
	const char* getAppPath();

	inline void SafeMemcpy(void* __restrict dest, const size_t max, const void* __restrict src, const size_t size){
		SLASSERT(size <= max, "over flow");
		memcpy(dest, src, (size <= max) ? size : max);
	}
	inline void SafeMemset(void* __restrict dest, const size_t max, const int val, const size_t size){
		SLASSERT(size <= max, "over flow");
		memset(dest, val, (size <= max) ? size : max);
	}
#ifdef __cplusplus
}
#endif

inline int32 CalcStringUniqueId(const char * str) {
	int32 seed = 131;
	int32 hash = 0;
	while (*str) {
		hash = hash * seed + (*str++);
	}
	return hash;
}

inline int32 getRandom(int32 side1, int32 side2){
	static std::random_device dev;
	static std::default_random_engine eng(dev());
	if (side1 > side2)
		std::swap(side1, side2);
	return std::uniform_int_distribution<int32>(side1, side2)(eng);
}


inline int64 getTimeTickOfNextMin(){
	time_t curTime = time(NULL);
	tm localTm = *localtime(&curTime);
	localTm.tm_sec = 0;
	
	return mktime(&localTm) * 1000 + 60 * 1000;
}

inline int64 getTimeTickOfNextHour(){
	time_t curTime = time(NULL);
	tm localTm = *localtime(&curTime);
	localTm.tm_sec = 0;
	localTm.tm_min = 0;

	return mktime(&localTm) * 1000 + 60 * 60 * 1000;
}

inline int64 getGameYearDay(){
	time_t now = time(NULL);
	now -= GAME_REFRESH_HOUR * 3600;
	tm localTm = *localtime(&now);

	return localTm.tm_yday;
}

inline int32 getGameYearWeek(){
	time_t now = time(NULL);
	time_t zero = 0;
	tm* zeroTm = localtime(&zero);
	now += zeroTm->tm_hour * 3600;
	now -= GAME_REFRESH_HOUR * 3600;
	int32 day = (int32)(now / 86400);

	return (zeroTm->tm_wday + day - 1) / 7 + 1;
}

inline int32 getGameMonth(){
	time_t now = time(NULL);
	now -= GAME_REFRESH_HOUR * 3600;
	tm* localTm = localtime(&now);

	return localTm->tm_year * 12 + localTm->tm_mon + 1;
}

}
#endif