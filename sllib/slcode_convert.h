//通用数据编解码转换
#ifndef _SL_CODE_CONVERT_H_
#define _SL_CODE_CONVERT_H_

#include "slbase_define.h"
#include "slsize_string.h"
#include "slbuffer.h"
#include <time.h>
#include "slarray.h"

#ifdef SL_OS_WINDOWS
	#include <WinSock2.h>
	#pragma  comment(lib,"ws2_32.lib")
#else
	#include <arpa/inet.h>
#endif



namespace sl
{
	//编解码方式
	enum CODE_TYPE
	{
		CODE_BIN = 1,
		CODE_STRING = 2,
		CODE_JSON = 3,
		CODE_MYSQL = 4,
		CODE_ENCRYPTED = 5, //加密协议，用于经过ptkey加密的数据包
	};

	//二进制编码
	struct bin_encode
	{};

	//二进制解码
	struct bin_decode
	{};

	//输出对象
	struct dump
	{};


	//可编码类的基类
	/*
		要用CCodeConvert进行编码的类都要从此类继承
	*/
	class CCodeObject
	{
	public:
		CCodeObject(): __len__(0), __max__(0), __ver__(0),__res__(0){}
		virtual ~CCodeObject(){}
	public:
		//成员变量
		int     __len__;             ///< 记录数据的长度
		int     __max__;             ///< 记录解析的最后哨位
		int     __ver__;             ///< 数据包的版本号
		int     __res__;             ///< 保留字段
	};

	///用于DataConvert的缓冲区类
	/*
		CodeStream自己不申请内存，是外部分配内存后传入到此类中
		@note 编码后的缓冲区不能超过64k
	*/
	class CCodeStream
	{
	public:
		char*		m_pszBuf;        ///< 指向Buffer的指针
		int			m_iBufSize;		 ///< Buffer总长度
		int			m_iPos;			 ///< Buffer当前游标
		int			m_iRet;			 ///< 编解码错误号
		void*		m_pReserve1;	 ///< 扩展字段1
		void*		m_pReserve2;	 ///< 扩展字段2
		int			m_pReserve3;	 ///< 扩展字段3
		int			m_pReserve4;	 ///< 扩展字段4
	
	public:
		CCodeStream()
			:m_pszBuf(0),
			 m_iBufSize(0),
			 m_iPos(0),
			 m_iRet(0),
			 m_pReserve1(0),
			 m_pReserve2(0),
			 m_pReserve3(0),
			 m_pReserve4(0)
		{}
		virtual ~CCodeStream(){}

		CCodeStream(const char* pszBuffer, int iBufSize)
			:m_iPos(0),
			 m_iRet(0),
			 m_pReserve1(0),
			 m_pReserve2(0),
			 m_pReserve3(0),
			 m_pReserve4(0)
		{
			Attach(pszBuffer, iBufSize);
		}

		CCodeStream(CBuffer& stBuf)
			:m_iPos(0),
			 m_iRet(0),
			 m_pReserve1(0),
			 m_pReserve2(0),
			 m_pReserve3(0),
			 m_pReserve4(0)
		{
			stBuf.Clear();
			Attach(stBuf.GetFreeBuf(), stBuf.GetFreeLen());
		}

		//绑定外部的内存缓冲区
		int Attach(const char* pszBuffer, int iBufSize)
		{
			SL_ASSERT( (pszBuffer != NULL && iBufSize > 0) );
			if(pszBuffer == NULL || iBufSize < 0)
			{
				return -1;
			}

			m_pszBuf    = const_cast<char *>(pszBuffer);
			m_iBufSize  = iBufSize;
			m_iPos      = 0;
			m_iRet      = 0;
			m_pReserve1 = 0;
			m_pReserve2 = 0;
			m_pReserve3 = 0;
			m_pReserve4 = 0;

			return 0;
		}

		//转换初始化
		/*
			开始转换时调用此函数
			用来把一些变量置为0
		*/
		void InitConvert()
		{
			m_iPos      = 0;
			m_iRet      = 0;
			m_pReserve1 = 0;
			m_pReserve2 = 0;
			m_pReserve3 = 0;
			m_pReserve4 = 0;
		}

		//向缓冲区附加内容
		/*
			@return 如果缓冲区不够就返回-1，否则返回向缓冲区写了多少字节
		*/
		int Append(const char* pszFormat, ...)
		{
			char* pszBuf = (char*)(m_pszBuf + m_iPos);
			int iBufSize = m_iBufSize - m_iPos;

			va_list ap;
			va_start(ap, pszFormat);
			int iRet = vsnprintf(pszBuf, iBufSize, pszFormat, ap);
			va_end(ap);

			if(iRet < 0)
			{
				return (m_iRet = -1);
			}
			m_iPos += iRet;
			return 0;
		}

		//向缓冲区附加二进制内容
		/*
			@return 如果缓冲区不够就返回-1，否则返回向缓冲区写了多少字节
		*/
		int AppendBinary(const char* pszString, int iLen)
		{
			if(pszString == NULL || ( (m_iBufSize - m_iPos) < iLen) )
			{
				return (m_iRet = -1);
			}

			memcpy(m_pszBuf + m_iPos, pszString, iLen);
			m_iPos += iLen;
			return 0;
		}
	}; // class CCodeStream

	class CCodeString: public ICodeRaw
	{
	protected:
		char*		m_pszBuf;
		int			m_iBufLen;

	public:
		CCodeString(char* pszBuf, int iBufLen)
			:m_pszBuf(pszBuf), m_iBufLen(iBufLen)
		{}
		
		virtual char* GetBuffer() const
		{
			return m_pszBuf;
		}

		virtual int GetBufferLen() const
		{
			return m_iBufLen;
		}

		virtual int GetDataLen() const
		{
			return (int)strlen(m_pszBuf);
		}

		virtual void SetDataLen(int iLen)
		{
			m_pszBuf[iLen] = '\0';
		}

	}; // class CCodeString

	class CCodeMemory: public ICodeRaw
	{
	protected:
		char*		m_pszBuf;
		int			m_iBufLen;
		int&		m_iDataLen;     ///< DataLen是外部变量

		//禁止赋值
		CCodeMemory& operator = (const CCodeMemory& obj)
		{
			return *this;
		}

	public:
		CCodeMemory(char* pszBuf, int iBufLen, int& iDataLen)
			:m_pszBuf(pszBuf), m_iBufLen(iBufLen), m_iDataLen(iDataLen)
		{

		}

		int Init(char* pszBuf, int iBufLen)
		{
			m_pszBuf   = pszBuf;
			m_iBufLen  = iBufLen;
			return 0;
		}

		virtual char* GetBuffer() const
		{
			return m_pszBuf;
		}

		virtual int GetBufferLen() const
		{
			return m_iBufLen;
		}

		virtual int GetDataLen() const
		{
			return m_iDataLen;
		}

		virtual void SetDataLen(int iLen)
		{
			m_iDataLen = iLen;
			m_pszBuf[iLen] = '\0';
		}

		int SetData(const char* pszBuf, int iDataLen)
		{
			if(iDataLen > GetBufferLen() || iDataLen < 0)
			{
				return -1;
			}
			memcpy(m_pszBuf, pszBuf, iDataLen);
			SetDataLen(iDataLen);
			return 0;
		}

	}; // class CCodeMemory


	//==========================================================
	#define BEGIN_CODE_CONVERT(ClassName) \
		template<typename METHOD> \
		int CodeConvert(CCodeStream& s, ClassName& t, const char* pszName, const METHOD& m) \
		{ \
			CodeConvertBegin(s, t, #ClassName, m); 
	
	#define CODE_CONVERT(iField) \
	        if(t.__max__ > s.m_iPos) \
			{ \
				CodeConvert(s, t.iField, #iField, m); \
			} 
    #define END_CODE_CONVERT(ClassName) \
			CodeConvertEnd(s, t, #ClassName, m); \
			return s.m_iRet; \
		}


	//原生类型的各种操作
	template<typename METHOD> int CodeConvertBegin(CCodeStream& s, CCodeObject& t, const char* pszName, const METHOD& m);
	template<typename METHOD> int CodeConvertEnd(CCodeStream& s, CCodeObject& t, const char* pszName, const METHOD& m);
	template<typename METHOD> int CodeConvertVersion(CCodeObject& s, int iVersion, const METHOD& m);
	
	template<typename METHOD> int CodeConvert(CCodeStream& s, char& t, const char* pszName, const METHOD& m);
	template<typename METHOD> int CodeConvert(CCodeStream& s, unsigned char& t, const char* pszName, const METHOD& m);
	template<typename METHOD> int CodeConvert(CCodeStream& s, short& t, const char* pszName, const METHOD& m);
	template<typename METHOD> int CodeConvert(CCodeStream& s, unsigned short& t, const char* pszName, const METHOD& m);
	template<typename METHOD> int CodeConvert(CCodeStream& s, int& t, const char* pszName, const METHOD& m);
	template<typename METHOD> int CodeConvert(CCodeStream& s, unsigned int& t, const char* pszName, const METHOD& m);
	template<typename METHOD> int CodeConvert(CCodeStream& s, int64& t, const char* pszName, const METHOD& m);
	template<typename METHOD> int CodeConvert(CCodeStream& s, uint64& t, const char* pszName, const METHOD& m);
	template<typename METHOD> int CodeConvert(CCodeStream& s, ICodeRaw& t, const char* pszName, const METHOD& m);

	//========================================================
	//bin_encode
	//检验缓冲区合法性
    #define SL_CHECK_BIN_ENCODE(s, size) \
		if(s.m_iRet != 0) \
		{ \
			return s.m_iRet; \
		} \
		if( (s.m_iPos + static_cast<int>(size)) > s.m_iBufSize ) \
		{ \
			return (s.m_iRet = -1); \
		}

	template<> inline
	int CodeConvert<bin_encode>(CCodeStream& s, char& t, const char* pszName, const bin_encode& m)
	{
		SL_CHECK_BIN_ENCODE(s, sizeof(t));

		*(char*)(s.m_pszBuf + s.m_iPos) = t;
		s.m_iPos += sizeof(t);
		return 0;
	}

	template<> inline
	int CodeConvert<bin_encode>(CCodeStream& s, unsigned char& t, const char* pszName, const bin_encode& m)
	{
		SL_CHECK_BIN_ENCODE(s, sizeof(t));

		*(unsigned char*)(s.m_pszBuf + s.m_iPos) = t;
		s.m_iPos += sizeof(t);
		
		return 0;
	}

	template<> inline
	int CodeConvert<bin_encode>(CCodeStream& s, short& t, const char* pszName, const bin_encode& m)
	{
		SL_CHECK_BIN_ENCODE(s, sizeof(t));

		*(short*)(s.m_pszBuf + s.m_iPos) = htons(static_cast<short>(t));
		s.m_iPos += sizeof(t);

		return 0;
	}

	template<> inline
	int CodeConvert<bin_encode>(CCodeStream& s, unsigned short& t, const char* pszName, const bin_encode& m)
	{
		SL_CHECK_BIN_ENCODE(s, sizeof(t));

		*(unsigned short*)(s.m_pszBuf + s.m_iPos) = htons(static_cast<unsigned short>(t));
		s.m_iPos += sizeof(t);

		return 0;
	}

	template<> inline
	int CodeConvert<bin_encode>(CCodeStream& s, int& t, const char* pszName, const bin_encode& m)
	{
		SL_CHECK_BIN_ENCODE(s, sizeof(t));

		*(int*)(s.m_pszBuf + s.m_iPos) = htonl(static_cast<int>(t));
		s.m_iPos += sizeof(t);

		return 0;
	}

	template<> inline
	int CodeConvert<bin_encode>(CCodeStream& s, unsigned int& t, const char* pszName, const bin_encode& m)
	{
		SL_CHECK_BIN_ENCODE(s, sizeof(t));

		*(unsigned int*)(s.m_pszBuf + s.m_iPos) = htonl(static_cast<unsigned int>(t));
		s.m_iPos += sizeof(t);

		return 0;
	}

	template<> inline
	int CodeConvert<bin_encode>(CCodeStream& s, int64& t, const char* pszName, const bin_encode& m)
	{
		SL_CHECK_BIN_ENCODE(s, sizeof(t));

		*(unsigned int*)(s.m_pszBuf + s.m_iPos) = htonl((unsigned int)((t >> 32) & 0xFFFFFFFF));
		s.m_iPos += sizeof(unsigned int);

		//再编低位的四个字节
		*(unsigned int*)(s.m_pszBuf + s.m_iPos) = htonl((unsigned int)(t & 0xFFFFFFFF));
		s.m_iPos += sizeof(unsigned int);

		return 0;
	}

	template<> inline
	int CodeConvert<bin_encode>(CCodeStream& s, uint64& t, const char* pszName, const bin_encode& m)
	{
		SL_CHECK_BIN_ENCODE(s, sizeof(t));
		
		//先编高位的四个字节
		*(unsigned int*)(s.m_pszBuf + s.m_iPos) = htonl((unsigned int)((t >> 32) & 0xFFFFFFFF));
		s.m_iPos += sizeof(unsigned int);

		///再编低位的四个字节
		*(unsigned int*)(s.m_pszBuf + s.m_iPos) = htonl((unsigned int)(t & 0xFFFFFFFF));
		s.m_iPos += sizeof(unsigned int);

		return 0;



	}

	template<> inline
	int CodeConvert(CCodeStream& s, ICodeRaw& t, const char* pszName, const bin_encode& m)
	{
		SL_CHECK_BIN_ENCODE(s, (sizeof(unsigned int) + t.GetDataLen()));

		//先编码长度
		int iLen = t.GetDataLen();
		*(unsigned int *)(s.m_pszBuf + s.m_iPos) = htonl(static_cast<unsigned int>(iLen));
		s.m_iPos += sizeof(int);

		//再编码内容
		s.AppendBinary(t.GetBuffer(), t.GetDataLen());

		return 0;
	}

	template<> inline
	int CodeConvertBegin<bin_encode>(CCodeStream& s, CCodeObject& t, const char* pszName, const bin_encode& m)
	{
		//编码len，但此时并不知道真正的长度，所以先把0编码进去
		//然后把m_iPos保存在__len__中
		t.__len__ = 0;
		t.__ver__ = s.m_iPos;
		CodeConvert<bin_encode>(s, t.__len__, pszName, m);
		t.__len__  = t.__ver__;  //先记录开始编码的位置

		//编码ver， 默认是0
		t.__ver__ = 0;
		CodeConvert<bin_encode>(s, t.__ver__, pszName, m);

		t.__max__ = s.m_iBufSize;  // 把__max__设到最大
		return 0;
	}

	template<> inline
	int CodeConvertEnd<bin_encode>(CCodeStream& s, CCodeObject& t, const char* pszName, const bin_encode& m)
	{
		//保存当前的m_iPos
		int iOldPos = s.m_iPos;

		//计算编码长度
		t.__len__ = s.m_iPos - t.__len__;

		//定位到头部
		s.m_iPos -= t.__len__;

		CodeConvert<bin_encode>(s, t.__len__, pszName, m);
		CodeConvert<bin_encode>(s, t.__ver__, pszName, m);

		//回复游标原来的位置
		s.m_iPos = iOldPos;
		return 0;
	}

	template<> inline
	int CodeConvertVersion<bin_encode>(CCodeObject& t, int iVersion, const bin_encode& m)
	{
		SL_ASSERT(t.__ver__ <= iVersion);
		t.__ver__ = iVersion;
		return 0;
	}


	//======================================================
	// bin_decode

	//检查缓冲区是否安全
    #define SL_CHECK_BIN_DECODE(s, size) \
		if(s.m_iRet != 0) \
		{ \
			return s.m_iRet; \
		} \
		if( (s.m_iPos + static_cast<int>(size)) > s.m_iBufSize ) \
		{ \
			return (s.m_iRet = -1); \
		}

	template<> inline
	int CodeConvert<bin_decode>(CCodeStream& s, char& t, const char* pszName, const bin_decode& m)
	{
		SL_CHECK_BIN_DECODE(s, sizeof(t));
		
		t = (char) s.m_pszBuf[s.m_iPos];
		s.m_iPos += sizeof(char);
		
		return 0;
	}

	template<> inline
	int CodeConvert<bin_decode>(CCodeStream& s, unsigned char& t, const char* pszName, const bin_decode& m)
	{
		SL_CHECK_BIN_DECODE(s, sizeof(t));
	
		t = (unsigned char)s.m_pszBuf[s.m_iPos];
		s.m_iPos += sizeof(char);

		return 0;
	}

	template<> inline
	int CodeConvert<bin_decode>(CCodeStream& s, short& t, const char* pszName, const bin_decode& m)
	{
		SL_CHECK_BIN_DECODE(s, sizeof(t));
		t = static_cast<short>( ntohs( *(unsigned short*)(s.m_pszBuf + s.m_iPos) ) );
		s.m_iPos += sizeof(t);

		return 0;
	}

	template<> inline
	int CodeConvert<bin_decode>(CCodeStream& s, unsigned short& t, const char* pszName, const bin_decode& m)
	{
		SL_CHECK_BIN_DECODE(s, sizeof(t));
		t = static_cast<unsigned short>( ntohs( *(unsigned short*)(s.m_pszBuf + s.m_iPos) ) );
		s.m_iPos += sizeof(t);

		return 0;
	}

	template<> inline
	int CodeConvert<bin_decode>(CCodeStream& s, int& t, const char* pszName, const bin_decode& m)
	{
		SL_CHECK_BIN_DECODE(s, sizeof(t));
		t =static_cast<int>( ntohl( *(unsigned int*)(s.m_pszBuf + s.m_iPos) ) );
		s.m_iPos += sizeof(t);

		return 0;
	}

	template<> inline
	int CodeConvert<bin_decode>(CCodeStream& s, unsigned int& t, const char* pszName, const bin_decode& m)
	{
		SL_CHECK_BIN_DECODE(s, sizeof(t));
		t = static_cast<unsigned int>( ntohl( *(unsigned int*)(s.m_pszBuf + s.m_iPos) ) );
		s.m_iPos += sizeof(t);

		return 0;
	}

	template<> inline
	int CodeConvert<bin_decode>(CCodeStream& s, int64& t, const char* pszName, const bin_decode& m)
	{
		SL_CHECK_BIN_DECODE(s, sizeof(t));

		//先解高位的四个字节
		t = ntohl(*(unsigned int*)(s.m_pszBuf + s.m_iPos));
		s.m_iPos += sizeof(unsigned int);
		t<<= 32;

		//再解低位的四个字节
		t += ntohl(*(unsigned int*)(s.m_pszBuf + s.m_iPos));
		s.m_iPos += sizeof(unsigned int);
		return 0;
	}

	template<> inline
	int CodeConvert<bin_decode>(CCodeStream& s, uint64& t, const char* pszName, const bin_decode& m)
	{
		SL_CHECK_BIN_DECODE(s, sizeof(t));

		//先解高位的四个字节
		t = ntohl(*(unsigned int*)(s.m_pszBuf + s.m_iPos));
		s.m_iPos += sizeof(unsigned int);
		t <<= 32;

		//再解低位的四个字节
		t += ntohl(*(unsigned int*)(s.m_pszBuf + s.m_iPos));
		s.m_iPos += sizeof(unsigned int);

		return 0;
	}
	
	//CSizeString要做特别处理
	template<int COUNT> inline
	int CodeConvert(CCodeStream& s, CSizeString<COUNT>& t, const char* pszName, const bin_decode& m)
	{
		//先解码长度
		SL_CHECK_BIN_DECODE(s, sizeof(unsigned int));
		int iLen  = ntohl( *(unsigned int*)(s.m_pszBuf + s.m_iPos) );
		s.m_iPos += sizeof(unsigned int);

		if(iLen >= t.GetBufferLen()) //不能是>
		{
			return (s.m_iRet = -3);
		}
		
		//再解码内容
		SL_CHECK_BIN_DECODE(s, iLen);
		memcpy(t.GetBuffer(), (s.m_pszBuf + s.m_iPos), iLen);
		s.m_iPos += iLen;
		t.SetDataLen(iLen);

		return 0;
	}

	template<> inline
	int CodeConvert<bin_decode>(CCodeStream& s, ICodeRaw& t, const char* pszName, const bin_decode& m)
	{
		//先解码长度
		SL_CHECK_BIN_DECODE(s, sizeof(unsigned int));
		int iLen = static_cast<int>( ntohl( *(unsigned int*)(s.m_pszBuf + s.m_iPos) ) );
		s.m_iPos += sizeof(unsigned int);

		if(iLen >= t.GetBufferLen())
		{
			return (s.m_iRet = -3);
		}

		//再解码内容
		SL_CHECK_BIN_DECODE(s, iLen);
		memcpy(t.GetBuffer(), (s.m_pszBuf + s.m_iPos), iLen);
		s.m_iPos += iLen;
		t.SetDataLen(iLen);

		return 0;
	}

	template<> inline
	int CodeConvertBegin<bin_decode>(CCodeStream& s, CCodeObject& t, const char* pszName, const bin_decode& m)
	{

		int iPos = s.m_iPos;
		CodeConvert<bin_decode>(s, t.__len__, pszName, m);
		t.__max__ = iPos + t.__len__;
		CodeConvert<bin_decode>(s, t.__ver__, pszName, m);
		
		return 0;
	}

	template<> inline
	int CodeConvertEnd<bin_decode>(CCodeStream& s, CCodeObject& t, const char* pszName, const bin_decode& m)
	{
		s.m_iPos = t.__max__; // 从高版本转到低版本时需要这样设置，以便跳过低版本没有的字段
		return 0;
	}

	template<> inline
	int CodeConvertVersion<bin_decode>(CCodeObject& t, int iVersion, const bin_decode& m)
	{
		// 存在新版本向旧版本转换的情况，需要这样处理
		if (t.__ver__ >= iVersion)
		{
			t.__res__ = iVersion;
			return 0;
		}
		else
		{
			return -1;
		}
	}



}// namespace sl

#endif