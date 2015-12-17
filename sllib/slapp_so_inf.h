//appsvrҵ���߼�so�Ľӿ���
#ifndef __SL_APP_SO_INF_H__
#define __SL_APP_SO_INF_H__

#include "slapp_inf.h"
#include "slmsg_base.h"
#include "slcheck_mac_def.h"
namespace sl
{
	///AppSo�ĵ���
	#define SL_APPSO_RAW	(CSingleton<detail::CVoidPtr,1>::Instance()->p)
	#define SL_APPSO		((CAppSoInf*)SL_APPSO_RAW)

	enum enumDataPathID
	{
		EDPID_CLIENT  = 1,       ///< ǰ��ͨѶ
		EDPID_DBSVR	  = 2,		 ///< ���ݿ�ͨѶ

		ESVRID_KVDB	  = 0,		 ///< KvdbSvr��SvrID
		ESVRID_OPENAPI = 1,		 ///< OpenAPISvr��SvrID
	};

	///ҵ�����ش�����̳У�ҵ��ֻ�ܿ���CAppSoInf,��������CAppInf
	class CAppSoInf
	{
	public:
		CAppSoInf()
		{
			SL_APPSO_RAW = this;
		}

		virtual ~CAppSoInf() {}
	
	public:

		///���ж�ȡ����
		virtual int OnInitConfig() = 0;

		//�ڴ�Ԥ����
		virtual int OnInitBuffer() = 0;

		//��ʼ����������
		virtual int OnInitOther() = 0;

		///AppInfÿ��ѭ��ʱ����
		virtual int OneRunOne() = 0;

		///AppInf�˳�ʱ����
		virtual int OnExit() = 0;
		
		///д��ͳ����Ϣ
		virtual int OnDumpStatInfo() = 0;

		///�û������߶������
		//�����û�����
		virtual void UserOffline(uid_t iUserID, int iReason, uint iRemoteIP) = 0;

		virtual void repeate_do() = 0;

	public:
		//��ͻ��˷�������
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

		///��ͻ��˷�������
		int SendToClient(CNetHead& stHead, CBuffer& stBuffer)
		{
			return SL_APP->SendData(EDPID_CLIENT, stHead, stBuffer.GetUsedBuf(), stBuffer.GetUsedLen());
		}

	public:
		///��ͻ��˷���Ӧ������
		template<typename T>
		int SendAnswer(CNetHead& stHead, MsgID_t stMsgID, int64 llAct, T& tData)
		{
			return SendToClient(stHead, stMsgID, llAct, tData, EMT_ANSWER);
		}

		///��ͻ��˷���ͨ�ô���
		int SendErrorAns(CNetHead& stHead, MsgID_t stMsgID, int64 llAct, CResult& stResult)
		{
			CErrorAns stAns;
			stAns.m_shMsgID  = stMsgID;
			stAns.m_stResult = stResult; 
			return SendAnswer<CErrorAns>(stHead, CErrorAns::CMD_ID, llAct, stAns);
		}

		///�������������������ݵ���غ���
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
		///ֱ�ӹرտͻ�������
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