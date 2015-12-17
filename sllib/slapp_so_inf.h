//appsvr业务逻辑so的接口类
#ifndef __SL_APP_SO_INF_H__
#define __SL_APP_SO_INF_H__

#include "slapp_inf.h"
#include "slmsg_base.h"
#include "slcheck_mac_def.h"
namespace sl
{
	///AppSo的单件
	#define SL_APPSO_RAW	(CSingleton<detail::CVoidPtr,1>::Instance()->p)
	#define SL_APPSO		((CAppSoInf*)SL_APPSO_RAW)

	enum enumDataPathID
	{
		EDPID_CLIENT  = 1,       ///< 前端通讯
		EDPID_DBSVR	  = 2,		 ///< 数据库通讯

		ESVRID_KVDB	  = 0,		 ///< KvdbSvr的SvrID
		ESVRID_OPENAPI = 1,		 ///< OpenAPISvr的SvrID
	};

	///业务主控从这里继承，业务只能看到CAppSoInf,而看不到CAppInf
	class CAppSoInf
	{
	public:
		CAppSoInf()
		{
			SL_APPSO_RAW = this;
		}

		virtual ~CAppSoInf() {}
	
	public:

		///集中读取配置
		virtual int OnInitConfig() = 0;

		//内存预分配
		virtual int OnInitBuffer() = 0;

		//初始化其他内容
		virtual int OnInitOther() = 0;

		///AppInf每次循环时调用
		virtual int OneRunOne() = 0;

		///AppInf退出时调用
		virtual int OnExit() = 0;
		
		///写入统计信息
		virtual int OnDumpStatInfo() = 0;

		///用户因离线而被清除
		//保存用户数据
		virtual void UserOffline(uid_t iUserID, int iReason, uint iRemoteIP) = 0;

		virtual void repeate_do() = 0;

	public:
		//向客户端发送数据
		template<typename T>
		int SendToClient(CNetHead& stHead, MsgID_t stMsgID, int64 llAct, T& tData, short sMsgType)
		{
			CMsgHead stMsg;
			stMsg.m_shMsgID		=	stMsgID;
			stMsg.m_shMsgType	=	sMsgType;
			stMsg.m_iCmdSeq		=	0;
			stMsg.m_llMsgAct	=	llAct;

			CCodeStream s(SL_APP->GetClientEncodeBuffer());
			int iRet = CodeConvert(s, stMsg, NULL, bin_decode());
			CHECK_RETURN(iRet);

			iRet = CodeConvert(s, tData, NULL, bin_decode());
			CHECK_RETURN(iRet);

			return SL_APP->SendData(EDPID_CLIENT, stHead, s.m_pszBuf, s.m_iPos);
		}

		///向客户端发送数据
		int SendToClient(CNetHead& stHead, CBuffer& stBuffer)
		{
			return SL_APP->SendData(EDPID_CLIENT, stHead, stBuffer.GetUsedBuf(), stBuffer.GetUsedLen());
		}

	public:
		///向客户端发送应答数据
		template<typename T>
		int SendAnswer(CNetHead& stHead, MsgID_t stMsgID, int64 llAct, T& tData)
		{
			return SendToClient(stHead, stMsgID, llAct, tData, EMT_ANSWER);
		}

		///向客户端发送通用错误
		int SendErrorAns(CNetHead& stHead, MsgID_t stMsgID, int64 llAct, CResult& stResult)
		{
			CErrorAns stAns;
			stAns.m_shMsgID  = stMsgID;
			stAns.m_stResult = stResult; 
			return SendAnswer<CErrorAns>(stHead, CErrorAns::CMD_ID, llAct, stAns);
		}

		///向其他服务器发送数据的相关函数
	public:
		template<typename T>
		int SendToServer(int iSvrID, CMsgHead& stHead, T& stData)
		{
			CBuffer& stCodeBuf = SL_APP->GetServerEncodeBuffer();
			CCodeStream s(stCodeBuf);
			s.InitConvert();
			int iRet = CodeConvert(s, stHead, NULL, bin_encode());
			CHECK_RETURN(iRet);

			iRet  = CodeConvert(s, stData, NULL, bin_encode());
			CHECK_RETURN(iRet);

			return SL_APP->SendToServer(iSvrID, stCodeBuf.GetUsedBuf(), stCodeBuf.GetUsedLen());
		}

	public:
		///直接关闭客户端连接
		static int CloseClientConnect(const CNetHead& a_stHead)
		{
			CNetHead stHead = a_stHead;
			stHead.m_LiveFlag = ENF_NET_FLAG_ASKCLOSE;
			stHead.m_iDataLength = 0;
			SL_APP->SendData(EDPID_CLIENT, stHead, NULL, 0);
			return 0;
		}
	};
}
#endif