#ifndef SL_PUBLIC_TOOLS_H
#define SL_PUBLIC_TOOLS_H
#include "slmulti_sys.h"
#ifdef SL_OS_WINDOWS
#include <shlwapi.h>
#endif // SL_OS_WINDOWS

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


}
#endif