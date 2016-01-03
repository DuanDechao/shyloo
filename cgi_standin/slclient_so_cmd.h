#ifndef __SLCLIENT_SO_CMD_H__
#define __SLCLIENT_SO_CMD_H__
#include "../sllib/slasync_cmd.h"
namespace sl
{
	template<class REQ, class ANS, int MSGID>
	class CClientSoCmd: public CAsyncCmd
	{
	public:
		enum {CMD_ID = MSGID};
		typedef REQ TReq;
		typedef ANS TAns;

		CClientSoCmd(): m_Uid(0) {}
		virtual ~CClientSoCmd() {}

		void Init(uid_t iUID, const TReq& tReq)
		{
			m_Uid		=	iUID;
			m_Req		=	tReq;
		}

		// pData时iSvrID的指针
		virtual int Do(void* pData)
		{
			CMsgHead stHead;
			stHead.m_shMsgID   = CMD_ID;
			stHead.m_shMsgType = EMT_REQUEST;
			stHead.m_iCmdSeq   = 1;
			stHead.m_llMsgAct  = (int64)this;

			int iRet = SL_CLIENT->SendToServer(stHead, m_Req);
			if(iRet != 0)
			{
				//SL_ERROR("SendToServer fail: ret=%d uid=%llu cmdid=%d", iRet, m_Uid, CMD_ID);
				return Done(iRet);
			}
			return 0;
		}

		virtual int OnAnswer(void* pData)
		{
			CCmdOnAnswerParam* pAnswerParam = (CCmdOnAnswerParam*)pData;
			if(pAnswerParam == NULL)
			{
				//SL_ERROR("Answer Param is NULL");
				return Done(-1);
			}

			if(pAnswerParam->m_stMsgHead.m_shMsgID != CMD_ID)
			{
				//SL_ERROR("error data from db svr, msg id(%d) error",pAnswerParam->m_stMsgHead.m_shMsgID);
				return Done(-1);
			}
			int iRet = CodeConvert(pAnswerParam->m_stCode, m_Ans, NULL, bin_decode());
			if(iRet != 0)
			{
				//SL_ERROR("Decode Data Errno %d", iRet);
				return Done(iRet);
			}
			//有委托时，以下两种情况执行委托
			//1.是全局委托
			//2.是对象委托自父命令存在，因为对象委托中的对象就是父命令
			/*if(!m_Delegate.isEmpty() && (m_Delegate.isStatic() || m_pstParent != NULL))
			{
				m_Delegate(iRet, this);
			}*/

			return Done(iRet);
		}
	public:
		uid_t						m_Uid;
		TReq						m_Req;
		TAns						m_Ans;
	};
}
#endif