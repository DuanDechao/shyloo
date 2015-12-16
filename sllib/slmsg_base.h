//������Ϣ����d����
#ifndef _SL_MSG_TYPE_H_
#define _SL_MSG_TYPE_H_
#include "sltype.h"
#include "slcode_convert.h"
namespace sl
{
	typedef ushort				MsgID_t;				///< ��ϢID����
	typedef short				MsgType_t;              ///< ��Ϣ���͵���������
	typedef uint				server_id_t;			///< ������ID

	//��Ϣ����
	enum enumMsgType
	{
		EMT_UNKNOW		=		0,			///< δ֪��Ϣ
		EMT_REQUEST		=		1,			///< ������Ϣ
		EMT_ANSWER		=		2,			///< ��Ӧ��Ϣ
		EMT_NOTIFY		=		3,			///< client->server֪ͨ��Ϣ
		EMT_ERRANS		=       4,			///< �򵥵Ĵ��󷵻�
	};

	//����ȫ����ϢID����
	enum enumMsgID
	{
		//�ڲ�ADMIN��Ϣ������
		MSGID_ADMIN_BEGIN		=		1,

		MSGID_ADMIN_END			=		998,
		MSGID_COMM_ERROR		=		999,    ///<ͨ�ô������ϢID

		//��¼��ϢID�����䣬��ܻ������¼�飺
		//�����ϢID���ǵ�¼��Ϣ�������û�е�¼���ͻ�ر�����
		MSGID_LOGIN_START		=		1001,
		MSGID_LOGIN_END			=		1005,

		MSGID_SET_DATA			=		1080,
	};

	//�ж���ϢID�ǲ���Admin��Ϣ
	inline bool IsAdminMsg(MsgID_t shMsgID)
	{
		return shMsgID >= MSGID_ADMIN_BEGIN && shMsgID <= MSGID_ADMIN_END;
	}

	//�ж��ǲ��ǵ�¼��ϢID
	inline bool isLoginMsg(MsgID_t shMsgID)
	{
		return shMsgID >= MSGID_LOGIN_START && shMsgID <= MSGID_LOGIN_END;
	}

	inline bool IsSetDataMsg(unsigned short shMsgID)
	{
		return shMsgID == MSGID_SET_DATA;
	}

	//ȫ�ִ����붨��
	enum enumRet
	{
		//ͨ�ô������1000��ʼ���
		RET_COMM_START				=			1000,
		RET_COMM_SYSTEM_ERROR		=			RET_COMM_START + 1,		///< 1001:ϵͳ�ڲ�����
		RET_COMM_INVALID_PARAM		=			RET_COMM_START + 2,		///< 1002:�ͻ��˷��صĲ�������
		RET_COMM_DATA_NOREADY		=			RET_COMM_START + 3,		///< 1003:���ݻ�δ׼����
		RET_COMM_DATA_LOADFAILED	=			RET_COMM_START + 4,		///< 1004:����׼��ʧ��
		RET_COMM_LOGIN_LIMITED		=			RET_COMM_START + 5,		///< 1005:���������ڲ���
		RET_COMM_LOGIN_ACCOUNT_LOCK =			RET_COMM_START + 6,     ///< 1006:�˺ű���
	};

	//��Ϣͷ��
	class CMsgHead: public CCodeObject
	{
	public:
		enum
		{
			EMAX_MSG_LENGTH		=		0x7FFF,        ///< ��Ϣ����󳤶�
		};

		MsgID_t			m_shMsgID;			///< ���������������Ϣ
		MsgType_t		m_shMsgType;			///< ��Ϣ���ͣ�����enumMsgType
		
		///��ϢSeq
		/*
			һ������£���ֵΪ0
			���첽ģʽ�£���Ҫ�����ֵ����ֹ���ͬID����õ��������Ӧ
			���������Ӧģʽ����Ч
		*/
		int				m_iCmdSeq;

		///ACT�ֶ�
		/*
			��Ҫԭ�ⲻ���Ļش���
			�������ø�ֵ����Ӧ��Ҫ��ش���ֵ
			���������Ӧģʽ����Ч��
		*/
		int64		   m_llMsgAct;

		CMsgHead()
			:m_shMsgID(0),
			 m_shMsgType(0),
			 m_iCmdSeq(0),
			 m_llMsgAct(0)
		{
		}

		CMsgHead(MsgID_t shMsgID, MsgType_t shMsgType = EMT_UNKNOW, int iCmdSeq = 0, int64 llMsgAct = 0)
			:m_shMsgID(shMsgID),
			 m_shMsgType(shMsgType),
			 m_iCmdSeq(iCmdSeq),
			 m_llMsgAct(llMsgAct)
		{
		}

		void Set(MsgID_t shMsgID = 0, MsgType_t shMsgType = EMT_UNKNOW, int iCmdSeq = 0, int64 llMsgAct = 0)
		{
			m_shMsgID		=	shMsgID;
			m_shMsgType		=	shMsgType;
			m_iCmdSeq		=	iCmdSeq;
			m_llMsgAct		=	llMsgAct;
		}

		bool IsValid() const
		{
			return m_shMsgID > 0 &&
				( m_shMsgType == EMT_REQUEST ||
				  m_shMsgType == EMT_ANSWER  ||
				  m_shMsgType == EMT_NOTIFY  ||
				  m_shMsgType == EMT_ERRANS);
		}
	};// class CMsgHead
	BEGIN_CODE_CONVERT(CMsgHead)
		CODE_CONVERT(m_shMsgID)
		CODE_CONVERT(m_shMsgType)
		CODE_CONVERT(m_llMsgAct)
		CODE_CONVERT(m_iCmdSeq)
	END_CODE_CONVERT(CMsgHead)

	// Net��Ϣ����ʽ����
	enum enumNetPackage
	{
		NET_PACKAGE_BINARY		=		0,			///< ������Э�飬ͷ2�ֽڱ�ʶ��Ϣ����
		NET_PACKAGE_STRING		=		1,			///< �ַ���Э�飬\n��β
		NET_PACKAGE_FLASH		=		2,			///< flash xmlsocketЭ�飬��\0��β
		NET_PACKAGE_ENCRYPTED   =		3,			///<�ַ�������Э�飬��Ϊ��������ʽ
	};

	/// net socket ״̬����
	enum enumNetSock
	{
		ENS_NET_SOCK_FREE		=		0,			///< ����socket
		ENS_NET_SOCK_ACCEPTED	=		1,			///< �����ӵı���socket
		ENS_NET_SOCK_MAX		=		2,			///< 
	};


	/*
		Handle��״̬��־����
		��8λ����net���ϲ㷵��״̬
		��8λ�����ϲ���net���Ϳ���
	*/

	enum enumNetFlag
	{
		ENF_NET_FALG_ERROR			=	0x1,		///< ����δ֪����
		ENF_NET_FLAG_REMOTECLOSE	=	0x2,		///< Զ�������ر�����
		ENF_NET_FLAG_IDLECLOSE		=	0x4,

		ENF_NET_FLAG_ASKCLOSE		=	0x100,		///< Handle�ڷ�����ɺ�ر�
		ENF_NET_FLAG_BUSY			=	0x200,		///< �ϲ㷱æ
		ENF_NET_FLAG_MSG_TOO_BIG	=	0x400,      ///< ��Ϣ̫��
	};


	//Netͷ��
	class CNetHead
	{
	public:
		CNetHead():
		  m_RemoteIP(0),
		  m_RemotePort(0),
		  m_LocalIP(0),
		  m_LocalPort(0),
		  m_Handle(0),
		  m_HandleSeq(0),
		  m_CreateTime(0),
		  m_LastTime(0),
		  m_Act1(0),
		  m_Act2(0),
		  m_LiveFlag(0),
		  m_EncodeMethod(0),
		  m_DecodeMethod(0),
		  m_iDataLength(0)
		{
		}
		void SetAct(uint64 llAct1, uint64 llAct2)
		{
			m_Act1	=  llAct1;
			m_Act2	=  llAct2;
		}
		
		CNetHead& operator= (const CNetHead& s)
		{
			m_RemoteIP		=	s.m_RemoteIP;
			m_RemotePort	=   s.m_RemotePort;
			m_LocalIP		=   s.m_LocalIP;
			m_LocalPort		=   s.m_LocalPort;
			m_Handle		=   s.m_Handle;
			m_HandleSeq		=   s.m_HandleSeq;
			m_CreateTime	=   s.m_CreateTime;
			m_LastTime		=   s.m_LastTime;
			m_Act1			=   s.m_Act1;
			m_Act2			=   s.m_Act2;
			m_LiveFlag		=   s.m_LiveFlag;
			m_EncodeMethod	=   s.m_EncodeMethod;
			m_DecodeMethod	=   s.m_DecodeMethod;
			m_iDataLength	=   s.m_iDataLength;
			memcpy(m_Key, s.m_Key, sizeof(m_Key));
			
			return *this;
		}
	
	public:
		uint		m_RemoteIP;		///< Զ�̿ͻ��˵�IP
		ushort		m_RemotePort;	///< Զ�̿ͻ��˵Ķ˿�
		uint		m_LocalIP;		///< �������˵�IP
		ushort		m_LocalPort;	///< �������˶˿�

		uint		m_Handle;
		uint		m_HandleSeq;    ///< ���к�
		uint		m_CreateTime;	///< socket����ʱ��
		uint		m_LastTime;	    ///< socket����Ծʱ��
		uint64		m_Act1;			///< ACTֵ
		uint64		m_Act2;			///< ACTֵ

		//����
		/*
			@see enumNetFlag
			0x00FF & m_LiveFlag	��ʾ����NetSvr������Ӧ�ò��ֵ
			0xFF00 & m_LiveFlag ��ʾ��Ӧ�ò㴫��NetSvr��ֵ
			֪ͨNetSvr���Ӧ�����ӶϿ�������Ϣ��ʱ������m_LiveFlag  = ENF_NET_FLAG_ASKCLOSE
		*/
		ushort		m_LiveFlag;		///< �μ�enumNetFlag
		byte		m_EncodeMethod; ///< ��Ϣ����뷽ʽ
		byte		m_DecodeMethod; ///< ��Ϣ����뷽ʽ
		int			m_iDataLength;	///< ���������ݳ���
		char		m_Key[20];		///< ���ڼ���Э��Ľ���
	
	};// class CNetHead


	//===============================================
	//��Ӧ�Ľ��ֵ
	enum
	{
		EMAX_RESULTMSG		=		201,		///< ���󷵻���Ŀ
	};

	class CResult: public CCodeObject
	{
	public:
		CResult(): iResultID(0) {}
		
		int iResultID; //0��ʾ��ȷ
		CSizeString<EMAX_RESULTMSG> szResultMsg;
	};
	BEGIN_CODE_CONVERT(CResult)
		CODE_CONVERT(iResultID)
		CODE_CONVERT(szResultMsg)
	END_CODE_CONVERT(CResult)

	//ͳһ���󷵻�
	class CErrorAns: public CCodeObject
	{
	public:
		enum { CMD_ID = MSGID_COMM_ERROR};  //ͨ�ô��󷵻ص���ϢID
		CErrorAns(): m_shMsgID(0) {}

		MsgID_t m_shMsgID;
		CResult m_stResult;
	};
	BEGIN_CODE_CONVERT(CErrorAns)
		CODE_CONVERT(m_shMsgID)
		CODE_CONVERT(m_stResult)
	END_CODE_CONVERT(CErrorAns)

	//==================================================
	//����MsgID����Ϣ����
	template<int MessageID, int MessageType>
	class CMsgBase: public CCodeObject
	{
	public:
		enum
		{
			MSG_ID		=	MessageID,
			MSG_TYPE	=	MessageType,	
		};
		
		virtual void Reset() {}
	};

	//������Ϣ���ͻ���
	template<int MessageID>
	class CReqMsgBase: public CMsgBase<MessageID, EMT_REQUEST> {};

	//��Ӧ��Ϣ���ͻ���
	template<int MessageID>
	class CAnsMsgBase: public CMsgBase<MessageID, EMT_ANSWER>
	{
	public:
		CAnsMsgBase(): m_iResult(0) {}

		virtual void Reset()
		{
			m_iResult = 0;
		}

		void setResult(int iResult)
		{
			m_iResult = iResult;
		}

		int getResult() const
		{
			return m_iResult;
		}

		int m_iResult;		///< ��Ӧ���

	};
	
	//֪ͨ��Ϣ���ͻ���
	template<int MessageID>
	class CNoticeMsgBase: public CMsgBase<MessageID, EMT_NOTIFY> {};

}// namespace sl

#endif