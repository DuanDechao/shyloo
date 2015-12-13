//���õĻ�������
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
	#pragma warning(disable: 4503)  // ���ι��ڳ����������ĳ��ȣ����Ʊ��ضϵ�WARNING
	#pragma warning(disable: 4786)  // ���ι��ں���������256��WARNING
	#pragma warning(disable: 4127)  // ���ι����������ʽ�ǳ�����WARNING
	#pragma warning(disable: 4996)  // ���ι��ڱ�����Ϊ�����_CRT_SECURE_NO_DEPRECATE��WARNING
	#pragma warning(disable: 4100)  // ���ι���δ���õ��βε�WARNING

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

//windows��linux��snprintf�ķ���ֵ��һ���������������
namespace sl
{
	enum
	{
		SL_PATH_MAX		=		0124,				///< ·������󳤶�
		SL_INT_MAX		=		0x7FFFFFF,          ///< int���͵����ֵ
	};
}


#define SL_STRSIZE(x)	x,sizeof(x)			///< ����snprintf�еļ򻯺�

#define SL_CEIL(a, b)  ((a) == 0 ? 0 : ((a) - 1) / (b) + 1) 
#define SL_UNUSED(x)						///< �����Щ�����ں�����û�õ�����������Ա��⾯��



//2��unsigned int��uint64��ת��
#define SL_UINT64_MAKE(high, low) ((uint64)(((unsigned int)((low) & 0xFFFFFFFF)) | ((uint64)((unsigned int)((high) & 0xFFFFFFFF))) << 32))
#define SL_UINT64_LOW(i)		  ((unsigned int) ((uint64)(i) & 0xFFFFFFFF))
#define SL_UINT64_HIGH(i)		  ((unsigned int) ((uint64)(i) >> 32))

#ifdef SL_OS_WINDOWS
	typedef int socklen_t;
	typedef unsigned int pid_t;
	//Windows socket�Ĵ�����ͺ����linux��һ����ͨ����ͳһ
	#define SL_ERRNO			GetLastError()
	#define SL_WSA_ERRNO		WSAGetLastError()
	#define SL_EWOULDBLOCK		WSAEWOULDBLOCK		//	10035
	#define SL_EINPROGRESS		WSAEINPROGRESS		//	10036
	#define SL_ETIME			WSAETIMEDOUT		//	10060
	#define SL_EINTR			WSAEINTR			//	10000
	#define SL_ECONNRESET		WSAECONNRESET		//	10054	������ʾԶ�̹ر�������
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

///����Windows��Linux����Щ��������
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