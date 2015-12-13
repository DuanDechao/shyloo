//常用的基本定义
#ifndef _SL_BASE_DEFINE_H_
#define _SL_BASE_DEFINE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include "slconfig.h"
using namespace std;

#ifdef SL_OS_WINDOWS
	#pragma warning(disable: 4503)  // 屏蔽关于超出修饰名的长度，名称被截断的WARNING
	#pragma warning(disable: 4786)  // 屏蔽关于函数名超过256的WARNING
	#pragma warning(disable: 4127)  // 屏蔽关于条件表达式是常量的WARNING
	#pragma warning(disable: 4996)  // 屏蔽关于被声明为否决的_CRT_SECURE_NO_DEPRECATE的WARNING
	#pragma warning(disable: 4100)  // 屏蔽关于未引用的形参的WARNING

	#ifndef _WINSOCKAPI_
	#define _WINSOCKAPI_
	#define ECONNREFUSED 10061
	#endif
	#include <Windows.h>
	#include <WinSock2.h>
	#include <io.h>
	#include <WinDef.h>
#else
	#include <fcntl.h>
	#include <ctype.h>
	#include <unistd.h>
	#include <sys/time.h>
#endif

//windows和linux的snprintf的返回值不一样，这里进行屏蔽
namespace sl
{
	enum
	{
		SL_PATH_MAX		=		0124,				///< 路径的最大长度
		SL_INT_MAX		=		0x7FFFFFF,          ///< int类型的最大值
	};
}


#define SL_STRSIZE(x)	x,sizeof(x)			///< 用在snprintf中的简化宏

#define SL_CEIL(a, b)  ((a) == 0 ? 0 : ((a) - 1) / (b) + 1) 
#define SL_UNUSED(x)						///< 如果有些参数在函数类没用到，用这个可以避免警告



//2个unsigned int和uint64的转换
#define SL_UINT64_MAKE(high, low) ((uint64)(((unsigned int)((low) & 0xFFFFFFFF)) | ((uint64)((unsigned int)((high) & 0xFFFFFFFF))) << 32))
#define SL_UINT64_LOW(i)		  ((unsigned int) ((uint64)(i) & 0xFFFFFFFF))
#define SL_UINT64_HIGH(i)		  ((unsigned int) ((uint64)(i) >> 32))

#ifdef SL_OS_WINDOWS
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
	#define GetNowProcessId	GetCurrentProcessId 

	#ifndef SL_INVALID_SOCKET
		#define	SL_INVALID_SOCKET	(SOCKET)(~0)
	#endif

#else  //not SL_OS_WINDOWS
	#define closesocket			close
	#define GetNowProcessId		getpid
	#define GetCurrentThreadId	pthread_self

	typedef int SOCKET;

	#ifndef SL_INVALID_SOCKET
		#define	SL_INVALID_SOCKET	(SOCKET)(~0)
	#endif
	#define SL_ERRNO			errno
	#define SL_WSA_ERRNO		errno
	#define SL_EWOULDBLOCK		EWOULDBLOCK
	#define SL_EINPROGRESS		EINPROGRESS
	#define SL_ETIME			ETIME
	#define SL_EINTR			EINTR
	#define SL_ECONNRESET		ECONNRESET
#endif

///屏蔽Windows和Linux的这些函数差异
#ifdef SL_OS_WINDOWS
	#define sl_fopen			CPlatForm::_sl_fopen
	#define sl_stat				_stat32
	#define sl_access			_access
	#define	sl_rename			rename
	#define sl_unlink			_unlink
	#define	sl_remove			remove
	#define strcasecmp          _stricmp
	#define strncasecmp         _strnicmp
#else
	#define sl_fopen			fopen
	#define sl_stat				stat
	#define sl_access			access
	#define	sl_rename			rename
	#define sl_unlink			unlink
	#define	sl_remove			remove
#endif

#define sl_vsnprintf			CPlatForm::_sl_vsnprintf	
#define sl_snprintf				CPlatForm::_sl_snprintf
#endif //_SL_BASE_DEFINE_H_