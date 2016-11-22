//ͨ�����ݱ����ת��
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
	//����뷽ʽ
	enum CODE_TYPE
	{
		CODE_BIN = 1,
		CODE_STRING = 2,
		CODE_JSON = 3,
		CODE_MYSQL = 4,
		CODE_ENCRYPTED = 5, //����Э�飬���ھ���ptkey���ܵ����ݰ�
	};

	//�����Ʊ���
	struct bin_encode
	{};

	//�����ƽ���
	struct bin_decode
	{};

	//�������
	struct dump
	{};


	//�ɱ�����Ļ���
	/*
		Ҫ��CCodeConvert���б�����඼Ҫ�Ӵ���̳�
	*/
	class CCodeObject
	{
	public:
		CCodeObject(): __len__(0), __max__(0), __ver__(0),__res__(0){}
		virtual ~CCodeObject(){}
	public:
		//��Ա����
		int     __len__;             ///< ��¼���ݵĳ���
		int     __max__;             ///< ��¼�����������λ
		int     __ver__;             ///< ���ݰ��İ汾��
		int     __res__;             ///< �����ֶ�
	};

	///����DataConvert�Ļ�������
	/*
		CodeStream�Լ��������ڴ棬���ⲿ�����ڴ���뵽������
		@note �����Ļ��������ܳ���64k
	*/
	class CCodeStream
	{
	public:
		char*		m_pszBuf;        ///< ָ��Buffer��ָ��
		int			m_iBufSize;		 ///< Buffer�ܳ���
		int			m_iPos;			 ///< Buffer��ǰ�α�
		int			m_iRet;			 ///< ���������
		void*		m_pReserve1;	 ///< ��չ�ֶ�1
		void*		m_pReserve2;	 ///< ��չ�ֶ�2
		int			m_pReserve3;	 ///< ��չ�ֶ�3
		int			m_pReserve4;	 ///< ��չ�ֶ�4
	
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

		//���ⲿ���ڴ滺����
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

		//ת����ʼ��
		/*
			��ʼת��ʱ���ô˺���
			������һЩ������Ϊ0
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

		//�򻺳�����������
		/*
			@return ��������������ͷ���-1�����򷵻��򻺳���д�˶����ֽ�
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

		//�򻺳������Ӷ���������
		/*
			@return ��������������ͷ���-1�����򷵻��򻺳���д�˶����ֽ�
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
		int&		m_iDataLen;     ///< DataLen���ⲿ����

		//��ֹ��ֵ
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


	//ԭ�����͵ĸ��ֲ���
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
	//���黺�����Ϸ���
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

		//�ٱ��λ���ĸ��ֽ�
		*(unsigned int*)(s.m_pszBuf + s.m_iPos) = htonl((unsigned int)(t & 0xFFFFFFFF));
		s.m_iPos += sizeof(unsigned int);

		return 0;
	}

	template<> inline
	int CodeConvert<bin_encode>(CCodeStream& s, uint64& t, const char* pszName, const bin_encode& m)
	{
		SL_CHECK_BIN_ENCODE(s, sizeof(t));
		
		//�ȱ��λ���ĸ��ֽ�
		*(unsigned int*)(s.m_pszBuf + s.m_iPos) = htonl((unsigned int)((t >> 32) & 0xFFFFFFFF));
		s.m_iPos += sizeof(unsigned int);

		///�ٱ��λ���ĸ��ֽ�
		*(unsigned int*)(s.m_pszBuf + s.m_iPos) = htonl((unsigned int)(t & 0xFFFFFFFF));
		s.m_iPos += sizeof(unsigned int);

		return 0;



	}

	template<> inline
	int CodeConvert(CCodeStream& s, ICodeRaw& t, const char* pszName, const bin_encode& m)
	{
		SL_CHECK_BIN_ENCODE(s, (sizeof(unsigned int) + t.GetDataLen()));

		//�ȱ��볤��
		int iLen = t.GetDataLen();
		*(unsigned int *)(s.m_pszBuf + s.m_iPos) = htonl(static_cast<unsigned int>(iLen));
		s.m_iPos += sizeof(int);

		//�ٱ�������
		s.AppendBinary(t.GetBuffer(), t.GetDataLen());

		return 0;
	}

	template<> inline
	int CodeConvertBegin<bin_encode>(CCodeStream& s, CCodeObject& t, const char* pszName, const bin_encode& m)
	{
		//����len������ʱ����֪�������ĳ��ȣ������Ȱ�0�����ȥ
		//Ȼ���m_iPos������__len__��
		t.__len__ = 0;
		t.__ver__ = s.m_iPos;
		CodeConvert<bin_encode>(s, t.__len__, pszName, m);
		t.__len__  = t.__ver__;  //�ȼ�¼��ʼ�����λ��

		//����ver�� Ĭ����0
		t.__ver__ = 0;
		CodeConvert<bin_encode>(s, t.__ver__, pszName, m);

		t.__max__ = s.m_iBufSize;  // ��__max__�赽���
		return 0;
	}

	template<> inline
	int CodeConvertEnd<bin_encode>(CCodeStream& s, CCodeObject& t, const char* pszName, const bin_encode& m)
	{
		//���浱ǰ��m_iPos
		int iOldPos = s.m_iPos;

		//������볤��
		t.__len__ = s.m_iPos - t.__len__;

		//��λ��ͷ��
		s.m_iPos -= t.__len__;

		CodeConvert<bin_encode>(s, t.__len__, pszName, m);
		CodeConvert<bin_encode>(s, t.__ver__, pszName, m);

		//�ظ��α�ԭ����λ��
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

	//��黺�����Ƿ�ȫ
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

		//�Ƚ��λ���ĸ��ֽ�
		t = ntohl(*(unsigned int*)(s.m_pszBuf + s.m_iPos));
		s.m_iPos += sizeof(unsigned int);
		t<<= 32;

		//�ٽ��λ���ĸ��ֽ�
		t += ntohl(*(unsigned int*)(s.m_pszBuf + s.m_iPos));
		s.m_iPos += sizeof(unsigned int);
		return 0;
	}

	template<> inline
	int CodeConvert<bin_decode>(CCodeStream& s, uint64& t, const char* pszName, const bin_decode& m)
	{
		SL_CHECK_BIN_DECODE(s, sizeof(t));

		//�Ƚ��λ���ĸ��ֽ�
		t = ntohl(*(unsigned int*)(s.m_pszBuf + s.m_iPos));
		s.m_iPos += sizeof(unsigned int);
		t <<= 32;

		//�ٽ��λ���ĸ��ֽ�
		t += ntohl(*(unsigned int*)(s.m_pszBuf + s.m_iPos));
		s.m_iPos += sizeof(unsigned int);

		return 0;
	}
	
	//CSizeStringҪ���ر���
	template<int COUNT> inline
	int CodeConvert(CCodeStream& s, CSizeString<COUNT>& t, const char* pszName, const bin_decode& m)
	{
		//�Ƚ��볤��
		SL_CHECK_BIN_DECODE(s, sizeof(unsigned int));
		int iLen  = ntohl( *(unsigned int*)(s.m_pszBuf + s.m_iPos) );
		s.m_iPos += sizeof(unsigned int);

		if(iLen >= t.GetBufferLen()) //������>
		{
			return (s.m_iRet = -3);
		}
		
		//�ٽ�������
		SL_CHECK_BIN_DECODE(s, iLen);
		memcpy(t.GetBuffer(), (s.m_pszBuf + s.m_iPos), iLen);
		s.m_iPos += iLen;
		t.SetDataLen(iLen);

		return 0;
	}

	template<> inline
	int CodeConvert<bin_decode>(CCodeStream& s, ICodeRaw& t, const char* pszName, const bin_decode& m)
	{
		//�Ƚ��볤��
		SL_CHECK_BIN_DECODE(s, sizeof(unsigned int));
		int iLen = static_cast<int>( ntohl( *(unsigned int*)(s.m_pszBuf + s.m_iPos) ) );
		s.m_iPos += sizeof(unsigned int);

		if(iLen >= t.GetBufferLen())
		{
			return (s.m_iRet = -3);
		}

		//�ٽ�������
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
		s.m_iPos = t.__max__; // �Ӹ߰汾ת���Ͱ汾ʱ��Ҫ�������ã��Ա������Ͱ汾û�е��ֶ�
		return 0;
	}

	template<> inline
	int CodeConvertVersion<bin_decode>(CCodeObject& t, int iVersion, const bin_decode& m)
	{
		// �����°汾��ɰ汾ת�����������Ҫ��������
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