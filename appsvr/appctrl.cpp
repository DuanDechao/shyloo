#include "../sllib/slsvr_base_frame.h"
#include "appconfig.h"

CAppCtrl g_App;		///全局对象
CAppCtrl::CAppCtrl()
	: CEpollAndShmSvr("appsvr"),
	  m_SecondCount(0),
	  m_MinitCount(0),
	  m_LastTime(0)
{
	REGISTERCTRL(this);
	
	///统计项描述
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
		"WriteBackCount",         ///< 回写完成的次数
		"WriteBackUsers",         ///< 回写的玩家个数
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
	///停止主循环，待Init执行完毕后再执行主循环
	m_InitFinish = false;

	//初始化数据缓冲区
	m_stClientEncodeBuf.Attach(NULL,1024, 0);
	m_stServerEncodeBuf.Attach(NULL, 1024, 0);
	m_stClientDecodeBuf.Attach(NULL, 1024, 0);


	///内存预分配
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
	if(m_LastTime == 0 || NowTime != m_LastTime)  //每秒执行一次
	{
		++m_SecondCount;
		m_LastTime = NowTime;

		if(m_SecondCount % 60 == 0) //每分钟执行一次
		{
			m_MinitCount++;
			m_SecondCount = 0;
			DumpStatInfo();
			if(m_MinitCount >= 10)
			{
				m_MinitCount = 0;
				//每十分钟写一次在线玩家数量日志
			}
		}

	}
	
	///定时检查命令
	SL_CMDFACTORY->CheckTimeoutCmd();

	//处理与前端管道中数据，防止数据包过多累积
	OnRecvData();

	///调用so的RunOnce

}
		