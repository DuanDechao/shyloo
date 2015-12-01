//appsoģʽ���첽����Ķ���
#ifndef _SL_APP_SO_CMD_H_
#define _SL_APP_SO_CMD_H_

#include "slasync_cmd.h"

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
			int iRet = CheckReq();
			if(iRet < 0) //�û�������û��ͨ��ϵͳ�����ִ���
			{
				return Done(iRet);
			}
			else if(iRet > 0)
			{
				return Done(iRet);
			}

			//����Ƿ��¼
			//if(!IsAdminMsg(CMD_ID) && !isLoginMsg(CMD_ID) && !)

			iRet = LoginDo(iRet);
			return iRet;
		}

	public:
		//�߼��������
		int LoginDo(int iRet)
		{
			if(iRet)
			{
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
				return Done(iRet);
			}


		}
	protected:
		//��һ�����Req�Ƿ�Ϸ���ִֻ��һ��
		virtual int CheckReq()	{return 0;}

		//��ʵ��ִ��ҵ���߼�DoWork()֮ǰ��׼�����ݣ����ܻ���ִ��
		virtual int PreData() {return 0;}
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
#endif