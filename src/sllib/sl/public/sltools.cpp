#include "sltools.h"
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
				memset(pSrcPath, 0, 256);
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
#ifdef __cplusplus
	}
#endif

}