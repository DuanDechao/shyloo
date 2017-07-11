#ifndef SL_LINUX_SYS_H
#define SL_LINUX_SYS_H
#include "slplatform.h"

#ifdef SL_OS_LINUX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/time.h>
#include <libgen.h>
#include <linux/limits.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <assert.h>
#include <signal.h>
#include <sys/socket.h>

#define LPVOID void
#define THREAD_FUN void*
#define ThreadID pthread_t

#define CSLEEP(n) usleep(n * 1000)
#define SafeSprintf snprintf
#define VSNPRINTF vsnprintf
#define GetNowProcessId		getpid
#define GetCurrentThreadId	pthread_self
#define SL_ERRNO			errno


#ifdef _DEBUG
#define SLASSERT(p, format, a...){   \
	char debug[4096] = {0};	\
	SafeSprintf(debug, sizeof(debug), format, ##a);	\
	((p) ? (void)0 : (void)__AssertionFail(__FILE__, __LINE__, __FUNCTION__, debug));	\
}
#else
#define SLASSERT(p, format, ...)
#endif

#ifdef _DEBUG
#define  ECHO(format, a...) {\
	char _log[4096] = {0};	\
	SafeSprintf(_log, 4096, "%s:%d:%s"#format, __FILE__, __LINE__, __FUNCTION__, ##a);	\
	printf("%s\n", _log);	\
}

#define ECHO_TRACE(format, a...){ \
	char _log[4096] = {0};	\
	SafeSprintf(_log, 4096, "%s:%d:%s"#format, __FILE__, __LINE__, __FUNCTION__, ##a);	\
	printf("%s\n", _log);	\
}

#define ECHO_ERROR(format, a...){ \
	char _log[4096] = {0};	\
	SafeSprintf(_log, 4096, "%s:%d:%s"#format, __FILE__, __LINE__, __FUNCTION__, ##a);	\
	printf("%s\n", _log);	\
}
#else
#define ECHO(format, a...)
#define ECHO_TRACE(format, a...)
#define ECHO_ERROR(format, a...)
#endif //_DEBUG

#define OUT
#define MAX_PATH 260
#define strtok_s strtok_r
#define stricmp strcasecmp
#define _stricmp strcasecmp
#define _access access
#endif

#endif
