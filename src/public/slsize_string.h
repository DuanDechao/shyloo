//定长的字符串封装
#ifndef _SL_SIZE_STRING_H_
#define _SL_SIZE_STRING_H_

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

namespace sl
{
	//字符串、二进制对象用于编解码接口类
	class ICodeRaw
	{
	public:
		virtual ~ICodeRaw(){}
		virtual char* GetBuffer() const = 0;
		virtual int GetBufferLen() const = 0;

		virtual int GetDataLen() const = 0;
		virtual void SetDataLen(int iLen) = 0;

	}; //class ICodeRaw

	//字符串接口
	class ISizeString: public ICodeRaw
	{
	public:
		virtual char* Get() const = 0;
		virtual int Size() const = 0;
		virtual int Set(const char* szFormat, ...) = 0;
		virtual int Cat(const char* szFormat, ...) = 0;

		bool operator == (const ISizeString& obj) const
		{
			return strcmp(Get(), obj.Get()) == 0;
		}

	}; //class ISizeString

	//定长的字符串封装类
	template<int SIZE = 1024>
	class CSizeString: public ISizeString
	{
	public:
		CSizeString()
		{
			m_szBuf[0] = 0;
		}

		CSizeString(const char* pszString)
		{
			Set("%s", pszString);
		}

		template<int S>
		CSizeString(const CSizeString<S>& pszString)
		{
			Set("%s", pszString.c_str());
		}

		virtual ~CSizeString(){}

		bool operator == (const char* pstr) const
		{
			return strcmp(Get(), pstr) == 0;
		}

		template<int S>
		bool operator == (const CSizeString<S>& pszString) const
		{
			return strcmp(c_str(), pszString.c_str()) == 0;
		}

		bool operator == (const ISizeString& obj) const
		{
			return strcmp(Get(), obj.Get()) == 0;
		}

		bool operator >= (const char* pstr) const
		{
			return strcmp(Get(), pstr) >= 0;
		}

		template<int S>
		bool operator >= (const CSizeString<S> & pszString) const
		{
			return strcmp(c_str(), pszString.c_str()) >= 0;
		}

		bool operator >= (const ISizeString& obj) const
		{
			return strcmp(Get(), obj.Get()) >= 0;
		}

		bool operator <= (const char* pstr) const
		{
			return strcmp(Get(), pstr) <= 0;
		}

		template<int S>
		bool operator <= (const CSizeString<S> & pszString) const
		{
			return strcmp(c_str(), pszString.c_str()) <= 0;
		}

		bool operator <= (const ISizeString& obj) const
		{
			return strcmp(Get(), obj.Get()) <= 0;
		}

		bool operator < (const char* pstr) const
		{
			return strcmp(Get(), pstr) >= 0;
		}

		template<int S>
		bool operator < (const CSizeString<S> & pszString) const
		{
			return strcmp(c_str(), pszString.c_str()) < 0;
		}

		bool operator < (const ISizeString& obj) const
		{
			return strcmp(Get(), obj.Get()) < 0;
		}

		bool operator > (const char* pstr) const
		{
			return strcmp(Get(), pstr) > 0;
		}

		template<int S>
		bool operator > (const CSizeString<S> & pszString) const
		{
			return strcmp(c_str(), pszString.c_str()) > 0;
		}

		bool operator > (const ISizeString& obj) const
		{
			return strcmp(Get(), obj.Get()) > 0;
		}

		bool operator != (const char* pstr) const
		{
			return strcmp(Get(), pstr) != 0;
		}

		template<int S>
		bool operator != (const CSizeString<S> & pszString) const
		{
			return strcmp(c_str(), pszString.c_str()) != 0;
		}

		bool operator != (const ISizeString& obj) const
		{
			return strcmp(Get(), obj.Get()) != 0;
		}

		//字符串比较
		int Compare(const char* pszString) const
		{
			return strcmp(c_str(), pszString);
		}

		template<int S>
		int Compare(const CSizeString<S>& obj) const
		{
			return strcmp(c_str(), obj.c_str());
		}

		int Compare(const ISizeString& obj) const
		{
			return strcmp(Get(), obj.Get());
		}

		ISizeString& operator = (const ISizeString& obj)
		{
			Copy(obj);
			return *this;
		}

		ISizeString& operator = (const char* pszString)
		{
			Copy(pszString);
			return *this;
		}

		template<int S>
		ISizeString& operator = (const CSizeString<S>& szString)
		{
			Set("%s", szString.c_str());
			return *this;
		}


	public:
		char* operator()() const
		{
			return Get();
		}
		const char* c_str() const
		{
			return m_szBuf;
		}

		const char& operator[] (int i) const
		{
			return m_szBuf[i];
		}

		char& operator[] (int i)
		{
			return m_szBuf[i];
		}
		
		virtual char* GetBuffer() const
		{
			return Get();
		}

		virtual int GetBufferLen() const
		{
			return Size();
		}

		virtual int GetDataLen() const
		{
			return (int) strlen(Get());
		}

		virtual void SetDataLen(int iLen)
		{
			m_szBuf[iLen] = '\0';
		}

		virtual char* Get() const
		{
			return (char*)m_szBuf;
		}

		virtual int Size() const
		{
			return sizeof(m_szBuf);
		}

		virtual int Set(const char* szFormat, ...)
		{
			va_list ap;
			va_start(ap, szFormat);
			int iRet = VSet(szFormat, ap);
			va_end(ap);
			return iRet;

		}

		virtual int VSet(const char* szFormat, va_list ap)
		{
			if(szFormat == NULL)
			{
				return -1;
			}

			int iRet = vsnprintf(m_szBuf, Size(), szFormat, ap);
			if(iRet < 0)
			{
				return -1;
			}
			m_szBuf[iRet] = 0;
			return iRet;
		}

		virtual int Cat(const char* szFormat, ...)
		{
			va_list ap;
			va_start(ap, szFormat);
			int iRet = VCat(szFormat, ap);
			va_end(ap);
			return iRet;
		}

		virtual int VCat(const char* szFormat, va_list ap)
		{
			if(szFormat == NULL)
			{
				return -1;
			}

			int iLen = GetDataLen();
			int iMaxLen = Size() - iLen;
			if(iMaxLen <= 0)
			{
				return -1;
			}

			int iRet = vsnprintf(m_szBuf + iLen, iMaxLen, szFormat, ap);
			if(iRet < 0)
			{
				return -1;
			}
			iRet += iLen;
			m_szBuf[iRet] = 0;
			return iRet;
		}

		int	Copy(const ISizeString& stString)
		{
			return Set("%s", stString.Get());
		}

		int Copy(const char* pszString)
		{
			return Set("%s", pszString);
		}

		void Clear()
		{
			m_szBuf[0] = '\0';
		}

	private:
		char    m_szBuf[SIZE];

	};// class CSizeString

} //namespace sl
#endif