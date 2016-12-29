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
	 const char* getAppPath(){
		static char* pSrcPath = nullptr;
		
		if (pSrcPath == nullptr){
			pSrcPath = NEW char[256];
			char link[256];
			memset(pSrcPath, 0 ,256);
			memset(link, 0, 256);

#ifdef SL_OS_WINDOWS
			GetModuleFileName(NULL, pSrcPath, 256);
			PathRemoveFileSpec(pSrcPath);
#endif

#ifdef SL_OS_LINUX
			SafeSprintf(link, sizeof(link), "/proc/self/exe");

			int nCount = readlink(link, pSrcPath, 256);
			if (nCount >= 265){
				SLASSERT(false, "system path error");
			}
			pSrcPath = dirname(pSrcPath);
#endif //linux
		}
		return pSrcPath;
	}

	inline void SafeMemcpy(void* __restrict dest, const size_t max, const void* __restrict src, const size_t size){
		SLASSERT(size <= max, "over flow");
		memcpy(dest, src, (size <= max) ? size : max);
	}
#ifdef __cplusplus
}
#endif


}
#endif