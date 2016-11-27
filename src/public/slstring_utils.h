//shyloo�ַ������ú�����װ��
#ifndef _SL_STRING_UTILS_
#define _SL_STRING_UTILS_
#include "slmulti_sys.h"
#include <vector>
#include <string>
using namespace std;
namespace sl
{
	//�ַ������ú�����
	class CStringUtils
	{
	public:

		/*
			ɾ���ַ�����ĳЩ�ַ�������ַ�
			@return	 ����ɾ���Ĵ���
		*/
		static int RemoveNot(std::string& str, const char* pszTrim)
		{
			size_t iIndex = str.find_first_not_of(pszTrim);
			int iCount = 0;
			while(iIndex != std::string::npos)
			{
				str.erase(iIndex, 1);
				iIndex = str.find_first_not_of(pszTrim);
				++iCount;
			}
			return iCount;
		}

		/*
			�ָ��ַ�������ز���
			���ַ����ָ�ɶ���ַ����������ж���ָ��
			@note ���ַ�ʽ������ظ��ķָ���
		*/
		static void Split(const string& strBuf, const string& strDel, std::vector<std::string>& aryStringList)
		{
			size_t pos1 = 0, pos2 = 0;
			while(true)
			{
				pos1 = strBuf.find_first_not_of(strDel, pos2);
				if(pos1 == string::npos)
				{
					break;
				}
				pos2 = strBuf.find_first_of(strDel, pos1);
				if(pos2 == string::npos)
				{
					aryStringList.push_back(strBuf.substr(pos1));
					break;
				}
				else
				{
					aryStringList.push_back(strBuf.substr(pos1, pos2-pos1));
				}
			}
		}

		/*
			�ָ�name = value ��
			@return �ָ�ɹ�����true
		*/
		static bool SplitIni(const string& strLine, string& strName, string& strValue, char chDel = '=')
		{
			if(strLine.size() == 0)
			{
				return false;
			}

			size_t iIndex = strLine.find(chDel);
			if(iIndex == string::npos)
			{
				return false;
			}
			strName = strLine.substr(0, iIndex);
			strValue = strLine.substr(iIndex + 1, strLine.size() - iIndex -1);
			
			return true;
		}

		///ɾ���ַ�������е�ĳЩ�ַ�
		static string& TrimLeft(string& str, const char* pszTrim)
		{
			size_t iIndex = str.find_first_not_of(pszTrim);
			if(iIndex != string::npos)
			{
				str.erase(0, iIndex);
			}

			return str;
		}

		//ɾ���ַ����ұ��е�ĳЩ�ַ�
		static string& TrimRight(string& str, const char* pszTrim)
		{
			size_t iIndex = str.find_last_not_of(pszTrim);
			if((++iIndex) != str.length())
			{
				str.erase(iIndex, str.length() - iIndex);
			}

			return str;
		}

		//תΪСд
		static string& MakeLower(string& str)
		{
			transform(str.begin(), str.end(), str.begin(), static_cast<int(*)(int)>(tolower));
			return str;
		}

		/**
        * �ϸ��ʮ��������ת��ģ�⺯��.
        * 1. ֧�ָ�ʽ[whitespace][sign]digits[whitespace]\n
        * 2. @����iLenΪ��ת�����ַ�������-1��ʾ��������\0��Ϊ������־\n
        * 3. ����ֵΪת��������֣����ת��ʧ�ܵĻ�����0����������piRet����\n
        * 4. NULL�Ϳմ�������Ϊ-1\n
        * 5. ���з�+-0123456789�ַ��������Ϊ-2\n
        * 6. ������ֵ��Χ������Ϊ-3\n
        */
        template<typename T>
        static T StrToInt(const char* pszStr, int iLen = -1, int* piRet = NULL)
        {
            const char* pszEnd = (iLen > 0 ? pszStr + iLen -1 : NULL);
            char* p = (char*) pszStr;
            if (p == NULL || *p == 0)
            {
                if (piRet) 
                { 
                    *piRet = -1; 
                } 
                return 0;
            }

            const bool bIsUnsigned = (((T)(-1)) > 0);
            T basedata = 1;
            T makedata = (T)-1;
            const T AbsMax = bIsUnsigned ? makedata : (basedata<<(sizeof(T)*8 - 1)^makedata);
    //        const T AbsMax = bIsUnsigned ? (T)(-1) : (T)((((T)(1))<<(GETTYPEBITSIZE(T)-1))^((T)(-1)));
    //        const T AbsMax = bIsUnsigned ? (T)(-1) : (T)(((T)(1))<<(sizeof(T)*8-1));
            const T Overflow = AbsMax / 10;

            // ���Կհ�
            while (*p == ' '&& (pszEnd ? p <= pszEnd : true))
            {
                ++p;
            }

            int sign = 0; // -1��ʾ�Ը��ſ�ʼ 1��ʾ�ԼӺſ�ʼ 0��ʾ�����ֿ�ʼ
            if (*p == '-' || *p == '+')
            {
                if (bIsUnsigned)
                {
                    if (piRet) 
                    { 
                        *piRet = -2; 
                    } 
                    return 0;
                }

                sign = (*p == '-' ? -1 : 1);
                ++p;
            }

            // ����0
            while (*p == '0' && (pszEnd ? p <= pszEnd : true))
            {
                ++p;
            }

            T ret = 0;
            for (; *p != 0 && (pszEnd ? p <= pszEnd : true); ++p)
            {
                if (*p >= '0' && *p <= '9')
                {
                    if (ret > Overflow) // ���������
                    {
                        if (piRet) 
                        { 
                            *piRet = -3; 
                        } 
                        return 0;
                    }
                    else if (ret == Overflow)
                    {
                        if (bIsUnsigned)
                        {
                            if (*p > '5')
                            {
                                if (piRet) 
                                { 
                                    *piRet = -3; 
                                } 
                                return 0;
                            }
                        }
                        else
                        {
                            if ((sign < 0 && *p > '8') || (sign >= 0 && *p > '7'))
                            {
                                if (*p > '5')
                                {
                                    if (piRet) 
                                    { 
                                        *piRet = -3; 
                                    } 
                                    return 0;
                                }
                            }
                        }
                    }

                    ret = ret * 10 + (*p - '0');
                }
                else if (*p == ' ')
                {
                    // �����ո�, ������������ǿո�źϷ�
                    for (; *p != 0 && (pszEnd ? p <= pszEnd : true); ++p)
                    {
                        if (*p != ' ')
                        {
                            if (piRet) 
                            { 
                                *piRet = -2; 
                            } 
                            return 0;
                        }
                    }
                    break;
                }
                else
                {
                    if (piRet) 
                    { 
                        *piRet = -2; 
                    } 
                    return 0;
                }
            }
            if(piRet)
            {
                *piRet = 0;
            }
            return (sign < 0 ? 0 - ret : ret);
        }
	};

}//namespace sl
#endif