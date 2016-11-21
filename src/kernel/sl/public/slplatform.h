//主要用来屏蔽Windows和Linux一些基本函数的差异
/********************************************************************
	created:	2015/12/05
	created:	5:12:2015   22:15
	filename: 	e:\myproject\shyloo\sllib\slplatform.h
	file path:	e:\myproject\shyloo\sllib
	file base:	slplatform
	file ext:	h
	author:		ddc
	
	purpose:	主要用来屏蔽Windows和Linux一些基本函数的差异
*********************************************************************/
#ifndef _SL_PLATFORM_H_
#define _SL_PLATFORM_H_
#include "slbase_define.h"

namespace sl
{
	//用来屏蔽Windows和Linux的一些基本函数的差异
	class CPlatForm
	{
	public:
		//Linux和windows的vsnprintf的函数的iCount和返回值处理有些不一样，这里统一
		//@return 同windows vsnprintf函数，缓冲区不足时返回-1

		static int _sl_vsnprintf(char* pszBuffer, int iCount, const char* pszFormat, va_list ap)
		{
#ifdef SL_OS_WINDOWS
			if(pszBuffer == NULL && iCount == 0)
			{
				return vsnprintf(NULL, 0, pszFormat, ap);
			}

			int iRet = vsnprintf(pszBuffer, iCount-1, pszFormat, ap);
#else

			int iRet = vsnprintf(pszBuffer, iCount, pszFormat, ap);
			if(pszBuffer == NULL && iCount == 0)
			{
				return iRet;
			}
			if(iRet >= iCount)
			{
				iRet = -1;
			}
#endif
			if(iRet > 0 && iRet < iCount)
			{
				pszBuffer[iRet] = 0;
			}
			va_end(ap);
			return iRet;
		}

		///linux 和windows的snprintf函数的iCount和返回值处理不一样，这里统一
		static int _sl_snprintf(char* pszBuffer, int iCount, const char* pszFormat, ...)
		{
			va_list ap;
			va_start(ap, pszFormat);
			int iRet = _sl_vsnprintf(pszBuffer, iCount, pszFormat, ap);
			va_end(ap);
			return iRet;
		}

#ifdef  SL_OS_WINDOWS
		///打开文件
		//在windows下用a/a+模式打开文件会报ERROR_ALREADY_EXISTS的错误，这里屏蔽一下
		static FILE* _sl_fopen(const char* pszFileName, const char* pszMode)
		{
			DWORD dwRet = GetLastError();
			FILE* pstFile = fopen(pszFileName, pszMode);
			SetLastError(dwRet);
			return pstFile;
		}
#endif
	};

} // namespace sl
#endif