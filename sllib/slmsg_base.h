//定义消息基本d定义
#ifndef _SL_MSG_TYPE_H_
#define _SL_MSG_TYPE_H_
#include "sltype.h"
#include "slcode_convert.h"
namespace sl
{
	typedef ushort				MsgID_t;				///< 消息ID类型
	typedef short				MsgType_t;              ///< 消息类型的数据类型
	typedef uint				server_id_t;			///< 服务器ID

	//消息类型
	enum enumMsgType
	{
		EMT_UNKNOW		=		0,			///< 未知消息
		EMT_REQUEST		=		1,			///< 请求消息
		EMT_ANSWER		=		2,			///< 响应消息
		EMT_NOTIFY		=		3,			///< client->server通知消息
		EMT_ERRANS		=       4,			///< 简单的错误返回
	};

	//几个全局消息ID定义
	enum enumMsgID
	{
		//内部ADMIN消息的区间
		MSGID_ADMIN_BEGIN		=		1,

		MSGID_ADMIN_END			=		998,
		MSGID_COMM_ERROR		=		999,    ///<通用错误的消息ID

		//登录消息ID的区间，框架会做如下检查：
		//如果消息ID不是登录消息，且玩家没有登录，就会关闭连接
		MSGID_LOGIN_START		=		1001,
		MSGID_LOGIN_END			=		1005,

		MSGID_SET_DATA			=		1080,
	};

	//判断消息ID是不是Admin消息
	inline bool IsAdminMsg(MsgID_t shMsgID)
	{
		return shMsgID >= MSGID_ADMIN_BEGIN && shMsgID <= MSGID_ADMIN_END;
	}

	//判断是不是登录消息ID
	inline bool isLoginMsg(MsgID_t shMsgID)
	{
		return shMsgID >= MSGID_LOGIN_START && shMsgID <= MSGID_LOGIN_END;
	}

	inline bool IsSetDataMsg(unsigned short shMsgID)
	{
		return shMsgID == MSGID_SET_DATA;
	}

	//全局错误码定义
	enum enumRet
	{
		//通用错误码从1000开始编号
		RET_COMM_START				=			1000,
		RET_COMM_SYSTEM_ERROR		=			RET_COMM_START + 1,		///< 1001:系统内部错误
		RET_COMM_INVALID_PARAM		=			RET_COMM_START + 2,		///< 1002:客户端返回的参数错误
		RET_COMM_DATA_NOREADY		=			RET_COMM_START + 3,		///< 1003:数据还未准备好
		RET_COMM_DATA_LOADFAILED	=			RET_COMM_START + 4,		///< 1004:数据准备失败
		RET_COMM_LOGIN_LIMITED		=			RET_COMM_START + 5,		///< 1005:服务器正在测试
		RET_COMM_LOGIN_ACCOUNT_LOCK =			RET_COMM_START + 6,     ///< 1006:账号被封
	};

	//消息头部
	class CMsgHead: public CCodeObject
	{
	public:
		enum
		{
			EMAX_MSG_LENGTH		=		0x7FFF,        ///< 消息的最大长度
		};

		MsgID_t			m_shMsgID;			///< 用来解析具体的消息
		MsgType_t		m_shMsgType;			///< 消息类型，来自enumMsgType
		
		///消息Seq
		/*
			一般情况下，该值为0
			在异步模式下，需要管理该值。防止多个同ID请求得到错误的响应
			在请求和响应模式下有效
		*/
		int				m_iCmdSeq;

		///ACT字段
		/*
			需要原封不动的回传。
			请求方设置该值，响应方要求回传该值
			在请求和响应模式下有效。
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

	// Net消息包格式定义
	enum enumNetPackage
	{
		NET_PACKAGE_BINARY		=		0,			///< 二进制协议，头2字节标识消息长度
		NET_PACKAGE_STRING		=		1,			///< 字符串协议，\n结尾
		NET_PACKAGE_FLASH		=		2,			///< flash xmlsocket协议，以\0结尾
		NET_PACKAGE_ENCRYPTED   =		3,			///<字符串加密协议，包为二进制形式
	};

	/// net socket 状态定义
	enum enumNetSock
	{
		ENS_NET_SOCK_FREE		=		0,			///< 空闲socket
		ENS_NET_SOCK_ACCEPTED	=		1,			///< 已连接的被动socket
		ENS_NET_SOCK_MAX		=		2,			///< 
	};


	/*
		Handle的状态标志定义
		低8位用于net向上层返回状态
		高8位用于上层向net发送控制
	*/

	enum enumNetFlag
	{
		ENF_NET_FALG_ERROR			=	0x1,		///< 其他未知错误
		ENF_NET_FLAG_REMOTECLOSE	=	0x2,		///< 远程主机关闭连接
		ENF_NET_FLAG_IDLECLOSE		=	0x4,

		ENF_NET_FLAG_ASKCLOSE		=	0x100,		///< Handle在发送完成后关闭
		ENF_NET_FLAG_BUSY			=	0x200,		///< 上层繁忙
		ENF_NET_FLAG_MSG_TOO_BIG	=	0x400,      ///< 消息太大
	};


	//Net头部
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
		uint		m_RemoteIP;		///< 远程客户端的IP
		ushort		m_RemotePort;	///< 远程客户端的端口
		uint		m_LocalIP;		///< 服务器端的IP
		ushort		m_LocalPort;	///< 服务器端端口

		uint		m_Handle;
		uint		m_HandleSeq;    ///< 序列号
		uint		m_CreateTime;	///< socket创建时间
		uint		m_LastTime;	    ///< socket最后活跃时间
		uint64		m_Act1;			///< ACT值
		uint64		m_Act2;			///< ACT值

		//活动标记
		/*
			@see enumNetFlag
			0x00FF & m_LiveFlag	表示是有NetSvr传过到应用层的值
			0xFF00 & m_LiveFlag 表示有应用层传到NetSvr的值
			通知NetSvr与对应的连接断开，发消息的时候设置m_LiveFlag  = ENF_NET_FLAG_ASKCLOSE
		*/
		ushort		m_LiveFlag;		///< 参见enumNetFlag
		byte		m_EncodeMethod; ///< 消息体编码方式
		byte		m_DecodeMethod; ///< 消息体解码方式
		int			m_iDataLength;	///< 后续的数据长度
		char		m_Key[20];		///< 用于加密协议的解密
	
	};// class CNetHead


	//===============================================
	//响应的结果值
	enum
	{
		EMAX_RESULTMSG		=		201,		///< 错误返回项目
	};

	class CResult: public CCodeObject
	{
	public:
		CResult(): iResultID(0) {}
		
		int iResultID; //0表示正确
		CSizeString<EMAX_RESULTMSG> szResultMsg;
	};
	BEGIN_CODE_CONVERT(CResult)
		CODE_CONVERT(iResultID)
		CODE_CONVERT(szResultMsg)
	END_CODE_CONVERT(CResult)

	//统一错误返回
	class CErrorAns: public CCodeObject
	{
	public:
		enum { CMD_ID = MSGID_COMM_ERROR};  //通用错误返回的消息ID
		CErrorAns(): m_shMsgID(0) {}

		MsgID_t m_shMsgID;
		CResult m_stResult;
	};
	BEGIN_CODE_CONVERT(CErrorAns)
		CODE_CONVERT(m_shMsgID)
		CODE_CONVERT(m_stResult)
	END_CODE_CONVERT(CErrorAns)

	//==================================================
	//带有MsgID的消息基类
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

	//请求消息类型基类
	template<int MessageID>
	class CReqMsgBase: public CMsgBase<MessageID, EMT_REQUEST> {};

	//响应消息类型基类
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

		int m_iResult;		///< 响应结果

	};
	
	//通知消息类型基类
	template<int MessageID>
	class CNoticeMsgBase: public CMsgBase<MessageID, EMT_NOTIFY> {};

}// namespace sl

#endif