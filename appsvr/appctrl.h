//应用控件类
/********************************************************************
	created:	2015/12/15
	created:	15:12:2015   20:04
	filename: 	d:\workspace\shyloo\appsvr\appctrl.h
	file path:	d:\workspace\shyloo\appsvr
	file base:	appctrl
	file ext:	h
	author:		ddc
	
	purpose:	应用控件类
*********************************************************************/
#ifndef __APP_CTRL_H__
#define __APP_CTRL_H__

#include "../sllib/slepoll_and_shm_svr_fram.h"
#include "../sllib/slbase.h"
#include "../sllib/slstat.h"
#include "../sllib/slapp_inf.h"
#include "../sllib/ipc/slshm_buff.h"
#include "../sllib/slapp_so_inf.h"
//#include <unistd.h>
#include <stdio.h>
using namespace sl;

class CAppCtrl: public CAppInf, public CEpollAndShmSvr
{
public:
	CAppCtrl();
	~CAppCtrl() {}

	enum enumAppStatID
	{
		app_stat_recvpkg = 0,
		app_stat_sendpkg,
		app_stat_accept,
		app_stat_idleclose,
		app_stat_mainloop,
		app_stat_waittimeout,
		app_stat_check,
		app_stat_toomoremsg,
		app_writeback_count,		///< 回写完成的次数
		app_writeback_users,		///< 回写的玩家个数
		app_stat_index_end,			///< 统计末端

	};

	enum
	{
		EMAX_SVRCONN_COUNT = 16,   ///< 最多能同时连接个后台服务
		EDCM_DOCODE_MAX    = 1000, ///< 一次处理的包的上限
	};

	///CAppInf要求实现的函数
public:
	///注册要预分配的内存
	/*
		@param [in] szBuffName	buffer名称
		@param [in] uiBuffSize	buffer大小
		@param [in] pInitFunc   初始化buffer的函数
	*/
	int RegisterBuffer(const char* szBuffName, unsigned int uiBuffSize, PInitBuffFunc pInitFunc)
	{
		return m_stShmBuff.RegisterBuff(szBuffName, uiBuffSize, pInitFunc);

	}

	//获取向前台发送消息的编码缓冲区
	CBuffer& GetClientEncodeBuffer()
	{
		return m_stClientEncodeBuf;
	}

	//写入数据
	int SendData(unsigned int uiDPKey, CNetHead& stHead, const char* pszBuf, int iBufLen);

	//获取向其他svr发送消息的编码缓冲区
	CBuffer& GetServerEncodeBuffer()
	{
		return m_stServerEncodeBuf;
	}

	int SendToServer(int iSvrID, const char* pszBuf, int iBufLen);   ///向后台发送数据

	//从CNetHead来判断用户是否已登录
	bool IsLogined(const CNetHead& stHead);

	void AttachUserToWB(sl::uid_t iUID);
	void DettachUserFromWB(sl::uid_t iUID);

	///CEpollAndShmSvr要求实现的函数
public:
	//加载配置项目
	virtual int LoadConfig();

	///重读配置项目
	virtual int ReLoadConfig();

	//具体业务的初始化
	virtual int WorkInit();

	///run循环中的一次执行过程
	virtual void RunOne();

	virtual void DoExit();

protected:
	int InitAppBuffer();
	
	//输出统计信息
	void DumpStatInfo();

	//检查后端的连接
	void CheckSvrConnect();

    ///处理管道中的数据包
	void OnRecvData(sl::uint uiPathKey);

	//接受用户请求
	void AcceptReq(CBuffer& stBuff, int iLen);

	//接受数据
	int RecvData(unsigned int uiDPKey, CBuffer& stBuff);

	int SetOfflining(const CNetHead& stHead);

public:
	///检查是否来自管理端口的消息
	static bool IsFromAdminPort(const CNetHead& stHead);

	//命令工厂的初始化函数
	static int InitCmdFactory(char* pszBuff, unsigned int uiSize, bool bResetShm);

	///事件回调函数
	static void OnClientEvent(unsigned int uiPathKey, int iEvent);

	static CAppCtrl* Instance()
	{
		return (CAppCtrl*) SL_APP;
	}

	bool IsExit() const
	{
		return m_bExit;
	}

	CStat* GetStat()
	{
		return &m_Stat;
	}

protected:
	bool					m_InitFinish;					///< 初始化是否完成

	unsigned int			m_SecondCount;
	unsigned int			m_MinitCount;						///< 分钟计数
	unsigned int			m_LastTime;							///< 上次记录时间

	CBuffer					m_stClientEncodeBuf;				///< Client编码缓冲区
	CBuffer					m_stClientDecodeBuf;				///< Client解码缓冲区
	CBuffer					m_stServerEncodeBuf;				///< Server编码缓冲区

	CStat					m_Stat;								///< 监控

	CSvrConnect				m_SvrConnect[EMAX_SVRCONN_COUNT];	///< 服務器連接

	CShmBuff				m_stShmBuff;						///< 共享内存数据缓冲区
};

#endif