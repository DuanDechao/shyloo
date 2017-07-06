#include "slmulti_sys.h"
#include <assert.h>
#include "sltime.h"
#ifdef __cplusplus
extern "C" {
#endif

FILE * fp = nullptr;
#ifdef __TRACE_MEMORY
#define TRACE_MEMORY(format, ...) { \
	OpenAllocLog(); \
	fprintf(fp, format, ##__VA_ARGS__); \
	fflush(fp); \
}
#else
#define TRACE_MEMORY(format, ...)
#endif


// void OpenAllocLog() {
	// if(fp == nullptr) {
		// char filename[260];
// #ifdef SL_OS_WINDOWS
		// SafeSprintf(filename, sizeof(filename), "alloc_%d.log", GetCurrentProcessId());
// #else
		// SafeSprintf(filename, sizeof(filename), "alloc_%d.log", getpid());
// #endif // SL_OS_WINDOWS
		// fopen(fp, filename, "a");
	// }
// }

void __AssertionFail(const char* file, int line, const char* funname, const char* debug){
	fflush(stdout);
	fprintf(stderr,
		"\nAssertion failed: %s\n=======assert string=======\nfile:%s\nline:%d\nfunction:%s\ndebug:%s\n",
		sl::getCurrentTimeStr().c_str(), file, line, funname, debug);
	fflush(stderr);
	assert(false);
}

void* __TraceMalloc(size_t size, const char* file, const size_t line, const char* func){
	void * ret = malloc(size);
	TRACE_MEMORY("malloc [%s:%d:%s] size %d => %lld\n", file, line, func, size, (uint64)ret);
	return ret;
}

void* __TraceRealloc(void * p, size_t size, const char* file, const size_t line, const char* func){
	void * ret = realloc(p, size);
	TRACE_MEMORY("realloc [%s:%d:%s] size %d => %lld[%lld]\n", file, line, func, size, (uint64)ret, (uint64)p);
	return ret;
}

void __TraceFree(void * p, const char * file, const size_t line, const char * func){
	free(p);
	TRACE_MEMORY("free [%s:%d:%s] => %lld\n", file, line, func, (uint64)p);
}

void _void_fun(){}

#ifdef SL_OS_WINDOWS
int32 __SafeSprintf(char * buf, int32 len, const char* format, ...){
	va_list argList;
	va_start(argList, format);
	uint32 ret = _vsnprintf_s(buf, len, _TRUNCATE, format, argList);
	va_end(argList);
	return ret;
}
#endif // SL_OS_WINDOWS

#ifdef __cplusplus
};
#endif