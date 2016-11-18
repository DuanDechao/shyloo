//��Ҫ��������Windows��LinuxһЩ���������Ĳ���
/********************************************************************
	created:	2015/12/05
	created:	5:12:2015   22:15
	filename: 	e:\myproject\shyloo\sllib\slplatform.h
	file path:	e:\myproject\shyloo\sllib
	file base:	slplatform
	file ext:	h
	author:		ddc
	
	purpose:	��Ҫ��������Windows��LinuxһЩ���������Ĳ���
*********************************************************************/
#ifndef _SL_PLATFORM_H_
#define _SL_PLATFORM_H_
#include "slbase_define.h"

namespace sl
{
	//��������Windows��Linux��һЩ���������Ĳ���
	class CPlatForm
	{
	public:
		//Linux��windows��vsnprintf�ĺ�����iCount�ͷ���ֵ������Щ��һ��������ͳһ
		//@return ͬwindows vsnprintf����������������ʱ����-1

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

		///linux ��windows��snprintf������iCount�ͷ���ֵ����һ��������ͳһ
		static int _sl_snprintf(char* pszBuffer, int iCount, const char* pszFormat, ...)
		{
			va_list ap;
			va_start(ap, pszFormat);
			int iRet = _sl_vsnprintf(pszBuffer, iCount, pszFormat, ap);
			va_end(ap);
			return iRet;
		}

#ifdef  SL_OS_WINDOWS
		///���ļ�
		//��windows����a/a+ģʽ���ļ��ᱨERROR_ALREADY_EXISTS�Ĵ�����������һ��
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