#include "../sllib/slsvr_base_frame.h"
#include "appconfig.h"
#include "../sllib/slasync_cmd.h"
CAppCtrl g_App;		///ȫ�ֶ���
CAppCtrl::CAppCtrl()
	: CEpollAndShmSvr("appsvr"),
	  m_SecondCount(0),
	  m_MinitCount(0),
	  m_LastTime(0)
{
	REGISTERCTRL(this);
	
	///ͳ��������
	const char szAppStatDesc[][CStatItem::SL_DESC_MAX] = 
	{
		"RecvMessage",
		"SendMessage",
		"Accept",
		"IdleClose",
		"MainLoop",
		"WaitTimeout",
		"Check",
		"TooMoreMsg",
		"WriteBackCount",         ///< ��д��ɵĴ���
		"WriteBackUsers",         ///< ��д����Ҹ���
	}
	m_Stat.Init(SL_COUNTOF(szAppStatDesc), szAppStatDesc);
}

int CAppCtrl::InitAppBuffer()
{
	size_t iSize = SL_CMDFACTORY->CountSize(APP_CONF->CmdFactoryConf.c_str());
	return RegisterBuffer("cmd factory", iSize, InitCmdFactory);
}

int CAppCtrl::SendToServer(int iSvrID, const char* pszBuf, int iBufLen)
{
	if(iSvrID < 0 || iSvrID >= (int)SL_COUNTOF())
}

int CAppCtrl::WorkInit()
{
	///ֹͣ��ѭ������Initִ����Ϻ���ִ����ѭ��
	m_InitFinish = false;

	//��ʼ�����ݻ�����
	m_stClientEncodeBuf.Attach(NULL,1024, 0);
	m_stServerEncodeBuf.Attach(NULL, 1024, 0);
	m_stClientDecodeBuf.Attach(NULL, 1024, 0);


	///�ڴ�Ԥ����
	int iRet = InitAppBuffer();
	SL_TRACE("appctrl init InitAppBuffer");

	iRet = m_stShmBuff.CreateBuff("key/appsvr.key");
	SL_TRACE("appctrl inti CreateBuff");

}

void CAppCtrl::RunOne()
{
	time_t		timep;
	struct tm   *pTime; 
	time(&timep);
	pTime = localtime(&timep);

	m_Stat.Put(app_stat_mainloop);
	sl::uint NowTime = static_cast<sl::uint>(time(0));
	if(m_LastTime == 0 || NowTime != m_LastTime)  //ÿ��ִ��һ��
	{
		++m_SecondCount;
		m_LastTime = NowTime;

		if(m_SecondCount % 60 == 0) //ÿ����ִ��һ��
		{
			m_MinitCount++;
			m_SecondCount = 0;
			DumpStatInfo();
			if(m_MinitCount >= 10)
			{
				m_MinitCount = 0;
				//ÿʮ����дһ���������������־
			}
		}

	}
	
	///��ʱ�������
	SL_CMDFACTORY->CheckTimeoutCmd();

	//������ǰ�˹ܵ������ݣ���ֹ���ݰ������ۻ�
	OnRecvData();

	///����so��RunOnce

}

void CAppCtrl::OnRecvData(sl::uint uiPathKey)
{
	int i = 0;
	for (; i < EDCM_DOCODE_MAX; ++i)
	{
		int iRecvLen = RecvData(uiPathKey, m_stClientDecodeBuf);
		if(iRecvLen <= 0)
		{
			//�]�Д������߫@ȡ����ʧ��
			break;
		}
		AcceptReq(m_stClientDecodeBuf, iRecvLen);
	}
	if(i >= EDCM_DOCODE_MAX)
	{
		m_Stat.Put(app_stat_toomoremsg);
		SL_WARNING("one time decode msg count is more than %d!", EDCM_DOCODE_MAX);
	}
}

void CAppCtrl::AcceptReq(CBuffer& stBuff, int iLen)
{
	if(m_bExit) //ֹͣ�˷��գ��@��̎��͑�����Ϣ
	{
		return;
	}

	//�є�����������
	CNetHead& stHead = *(CNetHead*)stBuff.GetUsedBuf();
	if(iLen != (int)sizeof(CNetHead) + stHead.m_iDataLength) //�L�Ȳ�һ��
	{
		SL_WARNING("code length invalid. %d != %d + %d",iLen, sizeof(CNetHead), stHead.m_iDataLength);
		return;
	}
	
	CCodeStream s;
	CMsgHead stMsgHead;

	//�ྀ�Д�
	if((stHead.m_LiveFlag && 0xFF) != 0)
	{
		SL_TRACE("user(%u) offline, liveflag = %x, handle = %d!",stHead.m_Act1, stHead.m_LiveFlag, stHead.m_Handle);
		SetOfflining(stHead);
		return;
	}

	int iRet = s.Attach(stBuff.GetUsedBuf() + sizeof(CNetHead), stHead.m_iDataLength);
	if(iRet)
	{
		return;
	}

	s.InitConvert();
	iRet = CodeConvert(s, stMsgHead, NULL, bin_decode());
	if(iRet)
	{
		return;
	}

	//�z����Ϣ�Ϸ���
	bool bIsAdmin = IsFromAdminPort(stHead);
	if(!bIsAdmin && IsAdminMsg(stMsgHead.m_shMsgID))
	{
		//����Ǐķǹ���˿ځ�Ĺ�����Ϣ�tֱ�Ӻ��Ե�
		SL_WARNING("user (%llu) send admin msg(%u) but no admin", static_cast<sl::uid_t>(stHead.m_Act1), stMsgHead.m_shMsgID);
		return;
	}
	
	//��̎��ęz�y
	SL_TRACE("get commend from user(%llu), cmd id = (%d)", stHead.m_Act1, stMsgHead.m_shMsgID);
	sl::uint iNowTime = static_cast<sl::uint>(time(0));
	if(iNowTime - stHead.m_LastTime >= 2)
	{
		m_Stat.Put(app_stat_waittimeout);
		SL_WARNING("user(%llu) cmd(%d) act(%d) waited more than 2 sec!", stHead.m_Act1, stMsgHead.m_shMsgID, stMsgHead.m_llMsgAct);
	}

	//���������Ю�������
	CAsyncCmdInf* pstCmd = SL_CMDFACTORY->CreateCmd(stMsgHead.m_shMsgID);
	if(pstCmd == NULL)
	{
		//��������ʧ��
		SL_WARNING("create cmd(%d) fails", stMsgHead.m_shMsgID);
		return;
	}
	CCmdDoParam stPara(stHead, stMsgHead, s, bIsAdmin);
	pstCmd->Do((void*)&stPara);
}