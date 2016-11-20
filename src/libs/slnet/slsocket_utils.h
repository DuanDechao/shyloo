//��װSocketһЩ���ò���
/********************************************************************
	created:	2015/12/01
	created:	1:12:2015   17:19
	filename: 	e:\myproject\shyloo\sllib\net\slsocket_utils.h
	file path:	e:\myproject\shyloo\sllib\net
	file base:	slsocket_utils
	file ext:	h
	author:		ddc
	
	purpose:	
*********************************************************************/
#ifndef _SL_SOCKET_UTILS_H_
#define _SL_SOCKET_UTILS_H_
#include "slconfig.h"
#ifdef SL_OS_WINDOWS
	#include <WinSock2.h>
	#pragma comment(lib, "ws2_32.lib")
	
	//WIN32��û��PF_LOCAL��Ϊ���ܱ��룬��PF_INET����
	#ifndef PF_LOCAL
		#define PF_LOCAL	PF_INET
	#endif
#else
	#include <unistd.h>
	#include <fcntl.h>
	#include <errno.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/un.h>
	#include <sys/ioctl.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <arpa/inet.h>
#endif
#include "../slsingleton.h"
#define		SL_ESOCKETEXIT	29999			///< �Լ�����ı�ʾҪ��socket�˳�
namespace sl
{
	//Socket���ú�����
	class CSocketUtils
	{
	public:
		static int NonblockSocket(SOCKET s)
		{
#ifndef SL_OS_WINDOWS
			int flag = 1;
			if(ioctl(s, FIONBIO, &flag) && 
				((flag = fcntl(s, F_GETFL, 0))< 0 || fcntl(s, F_SETFL, flag|O_NONBLOCK) < 0))
			{
				return errno;
			}
#else
			unsigned long ul = 1;
			if(ioctlsocket(s, FIONBIO, &ul))
			{
				return SL_WSA_ERRNO;
			}
#endif
			return 0;
		}

		//��ʱ�ĵ�λ�Ǻ���
		static int Select(SOCKET sock, bool rd, int iTimeOut)
		{
			struct timeval tv = {0};
			struct timeval tv2 = {0};
			struct timeval* ptv = NULL;

			int iLoop = 0;
			const int iSelectTime = 100; //100����

			if(iTimeOut < 0) //����
			{
				ptv = NULL;
				iLoop = 0x7FFFFFFF;
			}
			else
			{
				ptv = &tv2;
				if(iTimeOut >= iSelectTime)
				{
					iLoop = SL_CEIL(iTimeOut, iSelectTime);
					tv.tv_sec = iSelectTime / 1000;
					tv.tv_usec = (iSelectTime % 1000) * 1000;
				}
				else
				{
					iLoop = 1;
					tv.tv_sec  =  iTimeOut / 1000;
					tv.tv_usec = (iTimeOut % 1000) * 1000;
				}
			}

			fd_set fds;
			int iRet = 0;
			fd_set* prd = (rd ? &fds : NULL);
			fd_set* pwr = (rd ? NULL : &fds);
			int nfds = static_cast<int>(sock) + 1;
			for(; iLoop > 0; --iLoop)
			{
				if(ptv != NULL)
				{
					tv2.tv_sec = tv.tv_sec;
					tv2.tv_usec = tv.tv_usec;
				}

				FD_ZERO(&fds);
				FD_SET(sock, &fds);
				iRet = select(nfds, prd, pwr, NULL, ptv);
				if(iRet > 0)
				{
					return 0;
				}
				else if(iRet == 0)
				{
					continue;
				}
				else
				{
					if(SL_WSA_ERRNO == EINTR)
					{
						continue;
					}
					return SL_WSA_ERRNO;
				}
			}
			return SL_ETIME;

		}

		///�ж��Ƿ�������IP
		static bool IsLanIp(const char* pszIP)
		{
			if (pszIP == NULL)
			{
				return false;
			}

			return IsLanIp(inet_addr(pszIP));
		}

		///�ж��Ƿ�������IP
		static bool IsLanIp(unsigned int IP)
		{
			const unsigned int uiIP = htonl(IP);
			const unsigned int t = uiIP & 0xFFFF0000;
			const unsigned int t2 = uiIP & 0xFF000000;
			return (uiIP == 0 || uiIP == 0x7F000001 || t == 0xc0a80000 || t2 == 0xa000000 || (t >= 0xac100000 && t <= 0xac200000));
		}

	}; // class CSocketUtils

}// namespace sl
#endif