//��Ҫ��������Windows��LinuxһЩ���������Ĳ���

#ifndef _SL_PLATFORM_H_
#define _SL_PLATFORM_H_
#include <stdio.h>
namespace sl
{
	//��������Windows��Linux��һЩ���������Ĳ���
	class CPlatForm
	{
	public:
		//
		static int _sl_vsnprintf(char* pszBuffer, int iCount, const char* pszFormat, va_list ap)
		{
			/*int iRet = vsnprintf(pszBuffer, iCount, pszFormat, ap);
			if(pszBuffer == NULL && iCount == 0)
			{
				return iRet;
			}
			if(iRet >= iCount)
			{
				iRet = -1;
			}
			if(iRet >= 0 && iRet < iCount)
			{
				pszBuffer[iRet] = 0;
			}
			va_end(ap);
			return iRet;*/
			return 0;
		}
	};

} // namespace sl
#endif