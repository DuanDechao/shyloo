#ifndef SL_WINDOWS_SYS_H
#define SL_WINDOWS_SYS_H
#include "slplatform.h"

#ifdef SL_OS_WINDOWS
#define FD_SETSIZE 8096
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

#define CSLEEP(n) Sleep(n)
#define SafeSprintf __SafeSprintf
#define VSNPRINTF _vsnprintf
#define atoll _atoi64

typedef int socklen_t;
typedef unsigned int pid_t;

//Windows socket的错误码和含义和linux不一样，通过宏统一
#define SL_ERRNO			GetLastError()
#define SL_WSA_ERRNO		WSAGetLastError()
#define SL_EWOULDBLOCK		WSAEWOULDBLOCK		//	10035
#define SL_EINPROGRESS		WSAEINPROGRESS		//	10036
#define SL_ETIME			WSAETIMEDOUT		//	10060
#define SL_EINTR			WSAEINTR			//	10000
#define SL_ECONNRESET		WSAECONNRESET		//	10054	用来表示远程关闭了连接
#define GetNowProcessId		GetCurrentProcessId 

#ifndef SL_INVALID_SOCKET
#define	SL_INVALID_SOCKET	(SOCKET)(~0)
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