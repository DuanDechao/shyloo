#ifndef SL_MULTI_SYS_H
#define SL_MULTI_SYS_H
#include "sltype.h"
#include "slwin_sys.h"
#include "sllinux_sys.h"

#ifdef __cplusplus
extern "C"
{
#endif

	void __AssertionFail(const char* file, int line, const char* funname, const char* debug);
	void* __TraceMalloc(size_t size, const char* file, const size_t line, const char* func);
	void* __TraceRealloc(void * p, size_t size, const char* file, const size_t line, const char* func);
	void __TraceFree(void * p, const char * file, const size_t line, const char * func);

	typedef void(*VOID_FUN_TYPE)(void);
	void _void_fun();

#ifdef SL_OS_WINDOWS
	int32 __SafeSprintf(char * buf, int32 len, const char* format, ...);
#endif // SL_OS_WINDOWS

#ifdef __cplusplus
};
#endif

#define SECOND 1000
#define MINUTE 60 * SECOND
#define HOUR 60 * MINUTE
#define DAY 24 * HOUR

#define NEW new
#define DEL delete

#ifdef NEDMALLOC

#else
#define SLMALLOC(size) __TraceMalloc(size, __FILE__, __LINE__, __FUNCTION__)
#define SLREALLOC(p, size) __TraceRealloc(p, size, __FILE__, __LINE__, __FUNCTION__)
#define SLFREE(p) __TraceFree(p, __FILE__, __LINE__, __FUNCTION__)
#endif

#define SL_PATH_MAX 0124			///< 路径的最大长度
#define	SL_INT_MAX 0x7FFFFFF        ///< int类型的最大值
#define MAX_IP  256
#define MAX_BUF 1024

#define SL_STRSIZE(x)	x,sizeof(x)			///< 用在snprintf中的简化宏
#define SL_COUNTOF(x)  (sizeof(x)/sizeof(x[0]))
#define SL_MAX(a, b)   ((a) > (b) ? (a) : (b))
#define SL_MIN(a, b)   ((a) < (b) ? (a) : (b))
#define SL_CEIL(a, b)  ((a) == 0 ? 0 : ((a) - 1) / (b) + 1) 

#define SLShared_ptr	std::tr1::shared_ptr
#define SLUnordered_map std::tr1::unordered_map

/** 安全的释放一个指针内存 */
#define SAFE_RELEASE(i)										\
	if (i)													\
{													\
	delete i;										\
	i = NULL;										\
}

/** 安全的释放一个指针数组内存 */
#define SAFE_RELEASE_ARRAY(i)								\
	if (i)													\
{													\
	delete[] i;										\
	i = NULL;										\
}

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
	inline void safeMemcpy(void *__restrict __dest, size_t __max, const void *__restrict __src, size_t __n)
	{
		memcpy(__dest, __src, (__max >= __n) ? (__n) : (__max));
	}

	inline void safeMemset(void *__restrict __dest, size_t __max, int val, size_t __n)
	{
		memset(__dest, val, (__max >= __n) ? (__n) : (__max));
	}
#ifdef __cplusplus
};
#endif

#endif
