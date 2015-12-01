//appso模式下异步命令的定义
#ifndef _SL_APP_SO_CMD_H_
#define _SL_APP_SO_CMD_H_

#include "slasync_cmd.h"

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
			int iRet = CheckReq();
			if(iRet < 0) //用户的请求没有通过系统检测出现错误
			{
				return Done(iRet);
			}
			else if(iRet > 0)
			{
				return Done(iRet);
			}

			//检查是否登录
			//if(!IsAdminMsg(CMD_ID) && !isLoginMsg(CMD_ID) && !)

			iRet = LoginDo(iRet);
			return iRet;
		}

	public:
		//逻辑处理过程
		int LoginDo(int iRet)
		{
			if(iRet)
			{
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
				return Done(iRet);
			}


		}
	protected:
		//进一步检查Req是否合法，只执行一次
		virtual int CheckReq()	{return 0;}

		//在实际执行业务逻辑DoWork()之前，准备数据，可能会多次执行
		virtual int PreData() {return 0;}
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
#endif