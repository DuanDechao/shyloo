#ifndef SL_PUBLIC_TOOLS_H
#define SL_PUBLIC_TOOLS_H
#include "slmulti_sys.h"
#ifdef SL_OS_WINDOWS
#include <shlwapi.h>
#endif // SL_OS_WINDOWS
#include <random>

namespace sl
{
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

}
#endif