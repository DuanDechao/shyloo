#include "../sllib/slsvr_base_frame.h"
#include "appconfig.h"
#include "../sllib/slasync_cmd.h"
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

void CAppCtrl::OnRecvData(sl::uint uiPathKey)
{
	int i = 0;
	for (; i < EDCM_DOCODE_MAX; ++i)
	{
		int iRecvLen = RecvData(uiPathKey, m_stClientDecodeBuf);
		if(iRecvLen <= 0)
		{
			//沒有數據或者獲取數據失敗
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
	if(m_bExit) //停止了服務，這不處理客戶端信息
	{
		return;
	}

	//把數據解析出來
	CNetHead& stHead = *(CNetHead*)stBuff.GetUsedBuf();
	if(iLen != (int)sizeof(CNetHead) + stHead.m_iDataLength) //長度不一致
	{
		SL_WARNING("code length invalid. %d != %d + %d",iLen, sizeof(CNetHead), stHead.m_iDataLength);
		return;
	}
	
	CCodeStream s;
	CMsgHead stMsgHead;

	//斷線判斷
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

	//檢查消息合法性
	bool bIsAdmin = IsFromAdminPort(stHead);
	if(!bIsAdmin && IsAdminMsg(stMsgHead.m_shMsgID))
	{
		//如果是從非管理端口來的管理消息則直接忽略掉
		SL_WARNING("user (%llu) send admin msg(%u) but no admin", static_cast<sl::uid_t>(stHead.m_Act1), stMsgHead.m_shMsgID);
		return;
	}
	
	//慢處理的檢測
	SL_TRACE("get commend from user(%llu), cmd id = (%d)", stHead.m_Act1, stMsgHead.m_shMsgID);
	sl::uint iNowTime = static_cast<sl::uint>(time(0));
	if(iNowTime - stHead.m_LastTime >= 2)
	{
		m_Stat.Put(app_stat_waittimeout);
		SL_WARNING("user(%llu) cmd(%d) act(%d) waited more than 2 sec!", stHead.m_Act1, stMsgHead.m_shMsgID, stMsgHead.m_llMsgAct);
	}

	//創建并執行異步命令
	CAsyncCmdInf* pstCmd = SL_CMDFACTORY->CreateCmd(stMsgHead.m_shMsgID);
	if(pstCmd == NULL)
	{
		//創建命令失敗
		SL_WARNING("create cmd(%d) fails", stMsgHead.m_shMsgID);
		return;
	}
	CCmdDoParam stPara(stHead, stMsgHead, s, bIsAdmin);
	pstCmd->Do((void*)&stPara);
}