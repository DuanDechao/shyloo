#ifndef SL_WINDOWS_SYS_H
#define SL_WINDOWS_SYS_H
#include "slplatform.h"

#ifdef SL_OS_WINDOWS

#ifndef FD_SETSIZE
#define FD_SETSIZE 8096
#endif

#include <memory>
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#ifdef __cplusplus
#include <algorithm>
#endif

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "shlwapi.lib")

#define CSLEEP(n)			Sleep(n)
#define SafeSprintf			__SafeSprintf
#define VSNPRINTF			_vsnprintf
#define atoll				_atoi64
#define SL_ERRNO			GetLastError()
#define GetNowProcessId		GetCurrentProcessId

#ifdef _DEBUG
#define	SLASSERT(p, format, ...) { \
	char debug[4096] = {0}; \
	SafeSprintf(debug, sizeof(debug), format, ##__VA_ARGS__); \
	((p) ? (void)0 : (void)__AssertionFail(__FILE__, __LINE__, __FUNCTION__, debug)); \
}
#else
#define	SLASSERT(p, format, ...)
#endif

#define ECHO(format, ...){	\
	char _log[4096] = {0};	\
	SafeSprintf(_log, sizeof(_log), "%s:%d:%s"#format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);	\
	printf("%s\n", _log);	\
}

#define ECHO_TRACE(format, ...){\
	char _log[4096] = {0};	\
	SafeSprintf(_log, sizeof(_log), format, ##__VA_ARGS__);	\
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);	\
	printf("[trace]%s|%d|%s>>>%s\n", __FILE__, __LINE__, __FUNCTION__, _log);	\
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_INTENSITY);	\
}

#define ECHO_ERROR(format, ...){\
	char _log[4096] = {0};	\
	SafeSprintf(_log, sizeof(_log), format, ##__VA_ARGS__);		\
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);	\
	printf("[error]%s::%s\n", __FUNCTION__, _log);	\
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_INTENSITY);	\
}

#endif

#endif
