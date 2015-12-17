#include "appconfig.h"
#include "../sllib/slasync_cmd.h"
#include "appctrl.h"
#include "../sllib/slcheck_mac_def.h"
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
	};
	m_Stat.Init(SL_COUNTOF(szAppStatDesc), szAppStatDesc);
}

int CAppCtrl::InitAppBuffer()
{
	size_t iSize = SL_CMDFACTORY->CountSize(APP_CONF->CmdFactoryConf.c_str());
	return RegisterBuffer("cmd factory", iSize, InitCmdFactory);
}

int CAppCtrl::SendToServer(int iSvrID, const char* pszBuf, int iBufLen)
{
	if(iSvrID < 0 || iSvrID >= (int)SL_COUNTOF(m_SvrConnect) || pszBuf == NULL || iBufLen <= 0)
	{
		SL_ERROR("invalid param: svrid=%d, buf=%p, len=%d", iSvrID, pszBuf, iBufLen);
		return -1;
	}

	if(!m_SvrConnect[iSvrID].IsInited())
	{
		SL_ERROR("invalid Param: svrid=%d, buf=%p, len=%d, svr not inited",iSvrID, pszBuf, iBufLen);
		return -1;
	}

	return m_SvrConnect[iSvrID].Send(pszBuf, iBufLen);
}

int CAppCtrl::LoadConfig()
{
	return 0;
}

int CAppCtrl::ReLoadConfig()
{
	return 0;
}

bool CAppCtrl::IsFromAdminPort(const CNetHead& stHead)
{
	return true;
}

int CAppCtrl::SetOfflining(const CNetHead& stHead)
{
	return 0;
}

void CAppCtrl::DumpStatInfo()
{

}


int CAppCtrl::WorkInit()
{
	///停止主循环，待Init执行完毕后再执行主循环
	m_InitFinish = false;

	//初始化数据缓冲区
	m_stClientEncodeBuf.Attach(NULL, STREAM_BUFF_LENGTH, 0);
	m_stServerEncodeBuf.Attach(NULL, STREAM_BUFF_LENGTH, 0);
	m_stClientDecodeBuf.Attach(NULL, STREAM_BUFF_LENGTH, 0);

	///初始化後台連接
	int iRet = 0;
	bool NewServer = false;
	for (size_t i = 0; i < APP_CONF->SvrConnectInfo.size(); ++i)
	{
		CSvrConnectParam& stParam = APP_CONF->SvrConnectInfo[i];
		if(stParam.Index < 0 || stParam.Index >= EMAX_SVRCONN_COUNT)
		{
			SL_ERROR("invalid svr index %d, must in[0~%d]", stParam.Index, EMAX_SVRCONN_COUNT);
			return -1;
		}

		iRet = m_SvrConnect[stParam.Index].Init(GetEpoll(), stParam);
		CHECK_RETURN(iRet);

		iRet = m_SvrConnect[stParam.Index].ConnectSvr();
		CHECK_RETURN(iRet);
	}
	///内存预分配
	iRet = InitAppBuffer();
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

		CheckSvrConnect();

	}
	
	///定时检查命令
	SL_CMDFACTORY->CheckTimeoutCmd();

	//处理与前端管道中数据，防止数据包过多累积
	OnRecvData(EDPID_CLIENT);

	///调用so的RunOnce

}

void CAppCtrl::DoExit()
{

}

bool CAppCtrl::IsLogined(const CNetHead& stHead)
{
	return true;
}

void CAppCtrl::CheckSvrConnect()
{
	for (size_t i = 0; i< SL_COUNTOF(m_SvrConnect); ++i)
	{
		if(m_SvrConnect[i].IsInited())
		{
			m_SvrConnect[i].CheckConnect();
		}
	}
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

int CAppCtrl::RecvData(unsigned int uiDPKey, CBuffer& stBuff)
{
	//找到key对应的管道
	PCShmQ pstShm = GetShm(uiDPKey);
	if(pstShm == NULL)
	{
		SL_ERROR("shm %u fand fails!", uiDPKey);
		return -1;
	}
	else if(!pstShm->HasCode())
	{
		//管道里没有数据
		return 0;
	}
	m_Stat.Put(app_stat_recvpkg);
	stBuff.Clear();
	int iCodeLen = 0;

	//从管道里取出一个code
	int iRet = pstShm->GetOneCode(stBuff.GetFreeBuf(), stBuff.GetFreeLen(), iCodeLen);
	if(iRet || iCodeLen < (int)sizeof(CNetHead))
	{
		SL_ERROR("get one code but data fails (ret: %d, clen:%d)",iRet, iCodeLen);
		return -2;
	}
	stBuff.Append(iCodeLen);
	return iCodeLen;
}

int CAppCtrl::SendData(unsigned int uiDPKey, CNetHead& stHead, const char* pszBuf, int iBufLen)
{
	m_Stat.Put(app_stat_sendpkg);
	PCShmQ pstShm = GetShm(uiDPKey);
	if(pstShm == NULL)
	{
		SL_ERROR("shm %u find fails!", uiDPKey);
		return -1;
	}

	if((stHead.m_LiveFlag &  0xFF) != 0)
	{
		SL_WARNING("nethead (handle=%u, handlereq=%u) liveflag=%d not online, but want send data",
			stHead.m_Handle, stHead.m_HandleSeq, stHead.m_LiveFlag);
	}

	stHead.m_iDataLength = iBufLen;
	int iRet = pstShm->PutOneCode((const char*)&stHead, sizeof(stHead), pszBuf, iBufLen);
	if(iRet)
	{
		SL_ERROR("put one code into shm fails(%d)!", iRet);
	}
	return iRet;
}

int CAppCtrl::InitCmdFactory(char* pszBuff, unsigned int uiSize, bool bResetShm)
{
	SL_INFO("init cmd factory, config=%s", APP_CONF->CmdFactoryConf.c_str());
	return SL_CMDFACTORY->Init(APP_CONF->CmdFactoryConf.c_str(), pszBuff, uiSize & 0x7FFFFFFF);
}

void CAppCtrl::OnClientEvent(unsigned int uiPathKey, int iEvent)
{
	SL_TRACE("one net event(%d), data path Key(%u)", iEvent, uiPathKey);

	//处理管道中数据
	Instance()->OnRecvData(uiPathKey);
}

void CAppCtrl::AttachUserToWB(sl::uid_t iUID)
{

}

void CAppCtrl::DettachUserFromWB(sl::uid_t iUID)
{

}

