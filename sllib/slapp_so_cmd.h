//appsoģʽ���첽����Ķ���
#ifndef _SL_APP_SO_CMD_H_
#define _SL_APP_SO_CMD_H_

#include "slasync_cmd.h"
#include "slapp_inf.h"
#include "slapp_so_inf.h"
#include "sldelegate2.h"
namespace sl
{
	/*
		appsoģʽ�´�client msg��cmd����
		REQ/ANS���CReqMsgBase/CAnsMsgBase�̳�
	*/
	template <class REQ, class ANS, int MSGID>
	class CAppSoClientCmd: public CAsyncCmd
	{
	public:
		enum  {CMD_ID = MSGID,};   //���������ϢID
		typedef REQ TReq;
		typedef ANS TAns;

		CAppSoClientCmd(): m_iCmdSeq(0), m_llMsgAct(0), m_llUid(0), m_IsAnsClientUsed(false) {}
		virtual ~CAppSoClientCmd() {}

		int Do(void* pData)
		{
			if(pData == NULL)
			{
				return Done(RET_COMM_SYSTEM_ERROR);
			}
			CCmdDoParam& stData = *(CCmdDoParam*)pData;
			if(stData.m_stMsgHead.m_shMsgID != CMD_ID || stData.m_stMsgHead.m_shMsgType != EMT_REQUEST)
			{
				SL_ERROR("Cmd stat Err Cmd=%d CmdID=%d Tpye=%d", CMD_ID, stData.m_stMsgHead.m_shMsgID, stData.m_stMsgHead.m_shMsgType);
				return Done(RET_COMM_INVALID_PARAM);
			}

			//����
			int iRet = CodeConvert(stData.m_stCode, m_stReq, NULL, bin_decode());  //������������
			if(iRet)
			{
				return Done(RET_COMM_INVALID_PARAM);
			}

			m_stHead	=	stData.m_stHead;
			m_iCmdSeq	=	stData.m_stMsgHead.m_iCmdSeq;
			m_llMsgAct	=	stData.m_stMsgHead.m_llMsgAct;

			//m_llUid�Ǵ�NetSvr�ش���
			m_llUid		=	static_cast<uid_t>(m_stHead.m_Act1);

			//��������Ƿ�Ϸ�
			iRet = CheckReq();
			if(iRet < 0) //�û�������û��ͨ��ϵͳ�����ִ���
			{
				SL_ERROR("the require from client doesn't pass the system check(%d)", iRet);
				return Done(iRet);
			}
			else if(iRet > 0)  ///�û�������û��ͨ��ҵ���飬���ִ���
			{
				SL_ERROR("the require from client doesn't pass the work check(%d)", iRet);
;				return Done(iRet);
			}

			//����Ƿ��¼
			if(!IsAdminMsg(CMD_ID) && !isLoginMsg(CMD_ID) && !SL_APP->IsLogined(m_stHead)&& !IsSetDataMsg(CMD_ID))
			{
				SL_WARNING("user have not login!");
				CAppSoInf::CloseClientConnect(m_stHead);
				return Done(-1);
			}

			iRet = LoginDo(iRet);
			return iRet;
		}

	public:
		//�߼��������
		int LoginDo(int iRet)
		{
			if(iRet)
			{
				SL_ERROR("Cmd(%d) Logic Do fail(%d)!", CMD_ID, iRet);
				return Done(iRet);
			}

			//��׼�����ݵ�ʱ��һ��Ҫ���������Do����֮���ٰѸ�����
			//��������֮��
			iRet = PreData();
			if(iRet == RET_COMM_DATA_NOREADY)  //��������Ҫ׼����û�з���
			{
				return iRet;
			}
			else if(iRet)    //����׼����ʱ�����
			{
				SL_ERROR("cmd(%d) predata fail(%d)", CMD_ID, iRet);
				return Done(iRet);
			}

			iRet = CheckData();
			if(iRet < 0)  //���ݼ��û��ͨ��ϵͳ���
			{
				SL_ERROR("Cmd(%d) check data fails(%d)!", CMD_ID, iRet);
				return Done(iRet);
			}
			else if(iRet > 0) //���ݼ��û��ͨ��ҵ����
			{
				return Done(0);
			}

			iRet = DoWork();
			m_stAns.m_iResult = iRet;
			AnsClient();

			if(iRet > 0) ///ҵ�������ΪDoneʱ��Ĳ���
			{
				iRet = 0;
			}
			return Done(iRet);
		}
		void SetUid(uid_t Uid)
		{
			m_llUid  = Uid;
		}
		void SetNetHead(CNetHead& stHead)
		{
			m_stHead = stHead;
		}
	protected:
		//��һ�����Req�Ƿ�Ϸ���ִֻ��һ��
		virtual int CheckReq()	{return 0;}

		//��ʵ��ִ��ҵ���߼�DoWork()֮ǰ��׼�����ݣ����ܻ���ִ��
		virtual int PreData() {return 0;}

		///����׼����ɺ󣬼�������Ƿ�Ϸ���һ��ִֻ��һ��
		virtual int CheckData() {return 0;}

		///ҵ������̣�Ҫ����ͬ���ģ�һ���ִֻ��һ��
		///��Dowork���������AnsClient��LoginDo�Ѿ���æ������
		virtual int DoWork() { return 0;}

	protected:
		int AnsClient()
		{
			if(m_IsAnsClientUsed)
			{
				return 0;
			}
			m_IsAnsClientUsed = true;
			if(m_stAns.m_iResult)
			{
				CResult stResult;
				stResult.iResultID = m_stAns.m_iResult;
				return SL_APPSO->SendErrorAns(m_stHead, CMD_ID, m_llMsgAct, stResult);
			}
			else
			{
				return SL_APPSO->SendAnswer(m_stHead, CMD_ID, m_llMsgAct, m_stAns);
			}
		}
	protected:
		CNetHead		m_stHead;
		int				m_iCmdSeq;
		int64			m_llMsgAct;			///< ACT�ֶΣ���Ҫԭ�ⲻ���Ļش�
		uid_t			m_llUid;			///< �û�UID
		REQ				m_stReq;
		ANS				m_stAns;
		bool			m_IsAnsClientUsed;  ///< �Ƿ��Ѿ�����AnsClient��������Ӧ

	};
}
///��������ȷִ�к󣬻ص�ί�еĶ�����void callback(int& a_iRet, CAsyncCmd* a_pChildCmd)
SL_DELEGATE2(CDelegateCmdCallback, int& , CAsyncCmd*);
namespace sl
{
	///appsoģʽ�·�������server msg��cmd����
	// REQ/ANS ���CReqMsgBase/CAnsMsgBase�̳�
	template<class REQ, class ANS, int MSGID>
	class CAppSoServerCmd: public CAsyncCmd
	{
	public:
		enum {CMD_ID = MSGID};
		typedef REQ TReq;
		typedef ANS TAns;

		CAppSoServerCmd(): m_Uid(0) {}
		virtual ~CAppSoServerCmd() {}

		void Init(uid_t iUID, const TReq& tReq, CDelegateCmdCallback* pDelegate)
		{
			m_Uid		=	iUID;
			m_Req		=	tReq;
			if(pDelegate)
			{
				m_Delegate	=	pDelegate;
			}
		}

		// pDataʱiSvrID��ָ��
		virtual int Do(void* pData)
		{
			int iSvrIndex = *(int*)pData;

			CMsgHead stHead;
			stHead.m_shMsgID   = CMD_ID;
			stHead.m_shMsgType = EMT_REQUEST;
			stHead.m_iCmdSeq   = GetCmdSeq();
			stHead.m_llMsgAct  = (int64)this;

			int iRet = SL_APPSO->SendToServer(iSvrIndex, stHead, m_Req);
			if(iRet != 0)
			{
				SL_ERROR("SendToServer fail: ret=%d uid=%llu cmdid=%d", iRet, m_Uid, CMD_ID);
				return Done(iRet);
			}
			return 0;
		}

		virtual int OnAnswer(void* pData)
		{
			CCmdOnAnswerParam* pAnswerParam = (CCmdOnAnswerParam*)pData;
			if(pAnswerParam == NULL)
			{
				SL_ERROR("Answer Param is NULL");
				return Done(-1);
			}

			if(pAnswerParam->m_stMsgHead.m_shMsgID != CMD_ID)
			{
				SL_ERROR("error data from db svr, msg id(%d) error",pAnswerParam->m_stMsgHead.m_shMsgID);
				return Done(-1);
			}

			int iRet = CodeConvert(pAnswerParam->m_stCode, m_Ans, NULL,bin_decode());
			if(iRet != 0)
			{
				SL_ERROR("Decode Data Errno %d", iRet);
				return Done(iRet);
			}
			//��ί��ʱ�������������ִ��ί��
			//1.��ȫ��ί��
			//2.�Ƕ���ί���Ը�������ڣ���Ϊ����ί���еĶ�����Ǹ�����
			if(!m_Delegate.isEmpty() && (m_Delegate.isStatic() || m_pstParent != NULL))
			{
				m_Delegate(iRet, this);
			}

			return Done(iRet);
		}
	public:
		uid_t						m_Uid;
		CDelegateCmdCallback		m_Delegate;
		TReq						m_Req;
		TAns						m_Ans;
	};

	//===================================================================
	//��ȡ�����Ĳ�������
	///���������á�����AppSoServerCmd
	template<class TCMD>
	int DoServerCmd(CAsyncCmd& stAsyncCmd, int iSvrID,
		sl::uid_t iUID, const typename TCMD::TReq& stReq, CDelegateCmdCallback& stDelegate)
	{
		TCMD* pstCmd = (TCMD*)SL_CMDFACTORY->CreateCmd(TCMD::CMD_ID);
		if(pstCmd == NULL)
		{
			return stAsyncCmd.LogicDo(RET_COMM_SYSTEM_ERROR);
		}
		pstCmd->Init(iUID, stReq, &stDelegate);
		stAsyncCmd.RegisterChild(pstCmd);
		int a_iSvrID = iSvrID;
		int iRet = pstCmd->Do(&a_iSvrID);
		if(iRet != 0)
		{
			SL_WARNING("CCmdKetValueGet do fail: ret=%d",iRet);
			return RET_COMM_SYSTEM_ERROR;
		}
		return 0;
	}

}
#endif