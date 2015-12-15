#include "../sllib/slsvr_base_frame.h"
#include "appconfig.h"

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
		