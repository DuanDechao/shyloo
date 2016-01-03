//appso模式下异步命令的定义
#ifndef _SL_APP_SO_CMD_H_
#define _SL_APP_SO_CMD_H_

#include "slasync_cmd.h"
#include "slapp_inf.h"
#include "slapp_so_inf.h"
#include "sldelegate2.h"
namespace sl
{
	/*
		appso模式下从client msg的cmd基类
		REQ/ANS请从CReqMsgBase/CAnsMsgBase继承
	*/
	template <class REQ, class ANS, int MSGID>
	class CAppSoClientCmd: public CAsyncCmd
	{
	public:
		enum  {CMD_ID = MSGID,};   //该命令的消息ID
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

			//解码
			int iRet = CodeConvert(stData.m_stCode, m_stReq, NULL, bin_decode());  //解码请求数据
			if(iRet)
			{
				return Done(RET_COMM_INVALID_PARAM);
			}

			m_stHead	=	stData.m_stHead;
			m_iCmdSeq	=	stData.m_stMsgHead.m_iCmdSeq;
			m_llMsgAct	=	stData.m_stMsgHead.m_llMsgAct;

			//m_llUid是从NetSvr回传的
			m_llUid		=	static_cast<uid_t>(m_stHead.m_Act1);

			//检查请求是否合法
			iRet = CheckReq();
			if(iRet < 0) //用户的请求没有通过系统检测出现错误
			{
				SL_ERROR("the require from client doesn't pass the system check(%d)", iRet);
				return Done(iRet);
			}
			else if(iRet > 0)  ///用户的请求没有通过业务检查，出现错误
			{
				SL_ERROR("the require from client doesn't pass the work check(%d)", iRet);
;				return Done(iRet);
			}

			//检查是否登录
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
		//逻辑处理过程
		int LoginDo(int iRet)
		{
			if(iRet)
			{
				SL_ERROR("Cmd(%d) Logic Do fail(%d)!", CMD_ID, iRet);
				return Done(iRet);
			}

			//在准备数据的时候一定要在子命令的Do返回之后再把该命令
			//挂在命令之下
			iRet = PreData();
			if(iRet == RET_COMM_DATA_NOREADY)  //有数据需要准备还没有返回
			{
				return iRet;
			}
			else if(iRet)    //数据准备的时候出错
			{
				SL_ERROR("cmd(%d) predata fail(%d)", CMD_ID, iRet);
				return Done(iRet);
			}

			iRet = CheckData();
			if(iRet < 0)  //数据检查没有通过系统检查
			{
				SL_ERROR("Cmd(%d) check data fails(%d)!", CMD_ID, iRet);
				return Done(iRet);
			}
			else if(iRet > 0) //数据检查没有通过业务检查
			{
				return Done(0);
			}

			iRet = DoWork();
			m_stAns.m_iResult = iRet;
			AnsClient();

			if(iRet > 0) ///业务错误不作为Done时候的参数
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
		//进一步检查Req是否合法，只执行一次
		virtual int CheckReq()	{return 0;}

		//在实际执行业务逻辑DoWork()之前，准备数据，可能会多次执行
		virtual int PreData() {return 0;}

		///数据准备完成后，检查数据是否合法，一般只执行一次
		virtual int CheckData() {return 0;}

		///业务处理过程，要求是同步的，一般会只执行一次
		///在Dowork里无需调用AnsClient，LoginDo已经帮忙调用了
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
		int64			m_llMsgAct;			///< ACT字段，需要原封不动的回传
		uid_t			m_llUid;			///< 用户UID
		REQ				m_stReq;
		ANS				m_stAns;
		bool			m_IsAnsClientUsed;  ///< 是否已经调用AnsClient来发送响应

	};
}
///子命令正确执行后，回调委托的定义是void callback(int& a_iRet, CAsyncCmd* a_pChildCmd)
SL_DELEGATE2(CDelegateCmdCallback, int& , CAsyncCmd*);
namespace sl
{
	///appso模式下发给其他server msg的cmd基类
	// REQ/ANS 请从CReqMsgBase/CAnsMsgBase继承
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

		// pData时iSvrID的指针
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
			//有委托时，以下两种情况执行委托
			//1.是全局委托
			//2.是对象委托自父命令存在，因为对象委托中的对象就是父命令
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
	//抽取出来的操作函数
	///创建、设置、发送AppSoServerCmd
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