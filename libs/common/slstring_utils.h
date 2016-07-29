//shyloo字符串常用函数封装类
#ifndef _SL_STRING_UTILS_
#define _SL_STRING_UTILS_
#include "slbase_define.h"
#include "slplatform.h"
using namespace std;
namespace sl
{
	//字符串常用函数类
	class CStringUtils
	{
	public:

		/**
        * 格式化字符串.
        * 根据参数pszFormat字符串来转换并格式化数据，然后将结果复制到参数str所指的字符串中，直到字符串结束为止
        * @return 返回格式化后字符串的长度
        */
        static int Format(string& str, const char* pszFormat, ...)
        {
            va_list ap;
            va_start(ap, pszFormat);
            int iRet = FormatV(str, pszFormat, ap);
            va_end(ap);
            return iRet;
        }


		 /**
        * 格式化字符串.
        * 根据参数pszFormat字符串来转换并格式化数据，然后将结果复制到参数str所指的字符串中，直到字符串结束为止
        * @return 返回格式化后字符串的长度
        */
        static int FormatV(string& str, const char* pszFormat, va_list ap)
        {
            va_list apcopy;
#ifndef SL_OS_WINDOWS
            va_copy(apcopy, ap);
#else
            apcopy = ap;
            //assert(false && "函数 va_copy() 无法执行！");
#endif
            
            int iSize = sl_vsnprintf(NULL, 0, pszFormat, ap);   // 计算需要的内存空间
            if (iSize <= 0)
            {
                return iSize;
            }
            ++ iSize;

            int iLen = 0;
            char* pszBuf = new char[iSize];
            if (!pszBuf)
            {
                return -1;
            }        
            iLen = sl_vsnprintf(pszBuf, iSize, pszFormat, apcopy);
            va_end(apcopy);

            if (iLen < 0 || iLen >= iSize)
            {
                delete[] pszBuf;
                return iLen;
            }

            pszBuf[iLen] = 0;
            str.assign(pszBuf, iLen);
            delete[] pszBuf;

            return iLen;
        }


		/*
			删除字符串中某些字符串外的字符
			@return	 返回删除的次数
		*/
		static int RemoveNot(string& str, const char* pszTrim)
		{
			size_t iIndex = str.find_first_not_of(pszTrim);
			int iCount = 0;
			while(iIndex != string::npos)
			{
				str.erase(iIndex, 1);
				iIndex = str.find_first_not_of(pszTrim);
				++iCount;
			}
			return iCount;
		}

		/*
			分割字符串的相关操作
			把字符串分割成多个字符串，可以有多个分割符
			@note 这种方式会忽略重复的分隔符
		*/
		static void Split(const string& strBuf, const string& strDel, vector<string>& aryStringList)
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
			分割name = value 串
			@return 分割成功返回true
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

		///删除字符串左边中的某些字符
		static string& TrimLeft(string& str, const char* pszTrim)
		{
			size_t iIndex = str.find_first_not_of(pszTrim);
			if(iIndex != string::npos)
			{
				str.erase(0, iIndex);
			}

			return str;
		}

		//删除字符串右边中的某些字符
		static string& TrimRight(string& str, const char* pszTrim)
		{
			size_t iIndex = str.find_last_not_of(pszTrim);
			if((++iIndex) != str.length())
			{
				str.erase(iIndex, str.length() - iIndex);
			}

			return str;
		}

		//转为小写
		static string& MakeLower(string& str)
		{
			transform(str.begin(), str.end(), str.begin(), static_cast<int(*)(int)>(tolower));
			return str;
		}

		/**
        * 字符串比较，不区分大小写
        */
        static int CompareNoCase(const string& str1, const string& str2)
        {
            return strcasecmp(str1.c_str(), str2.c_str());
        }

		/**
        * 严格的十进制整数转换模扳函数.
        * 1. 支持格式[whitespace][sign]digits[whitespace]\n
        * 2. @参数iLen为待转化的字符个数，-1表示不定长以\0作为结束标志\n
        * 3. 返回值为转换后的数字，如果转换失败的话返回0，错误码由piRet接收\n
        * 4. NULL和空串错误码为-1\n
        * 5. 含有非+-0123456789字符会错误码为-2\n
        * 6. 超过数值范围错误码为-3\n
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

            // 忽略空白
            while (*p == ' '&& (pszEnd ? p <= pszEnd : true))
            {
                ++p;
            }

            int sign = 0; // -1表示以负号开始 1表示以加号开始 0表示以数字开始
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

            // 忽略0
            while (*p == '0' && (pszEnd ? p <= pszEnd : true))
            {
                ++p;
            }

            T ret = 0;
            for (; *p != 0 && (pszEnd ? p <= pszEnd : true); ++p)
            {
                if (*p >= '0' && *p <= '9')
                {
                    if (ret > Overflow) // 整型溢出了
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
                    // 遇到空格, 则后续都必须是空格才合法
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