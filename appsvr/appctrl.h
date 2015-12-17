//Ӧ�ÿؼ���
/********************************************************************
	created:	2015/12/15
	created:	15:12:2015   20:04
	filename: 	d:\workspace\shyloo\appsvr\appctrl.h
	file path:	d:\workspace\shyloo\appsvr
	file base:	appctrl
	file ext:	h
	author:		ddc
	
	purpose:	Ӧ�ÿؼ���
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
		app_writeback_count,		///< ��д��ɵĴ���
		app_writeback_users,		///< ��д����Ҹ���
		app_stat_index_end,			///< ͳ��ĩ��

	};

	enum
	{
		EMAX_SVRCONN_COUNT = 16,   ///< �����ͬʱ���Ӹ���̨����
		EDCM_DOCODE_MAX    = 1000, ///< һ�δ���İ�������
	};

	///CAppInfҪ��ʵ�ֵĺ���
public:
	///ע��ҪԤ������ڴ�
	/*
		@param [in] szBuffName	buffer����
		@param [in] uiBuffSize	buffer��С
		@param [in] pInitFunc   ��ʼ��buffer�ĺ���
	*/
	int RegisterBuffer(const char* szBuffName, unsigned int uiBuffSize, PInitBuffFunc pInitFunc)
	{
		return m_stShmBuff.RegisterBuff(szBuffName, uiBuffSize, pInitFunc);

	}

	//��ȡ��ǰ̨������Ϣ�ı��뻺����
	CBuffer& GetClientEncodeBuffer()
	{
		return m_stClientEncodeBuf;
	}

	//д������
	int SendData(unsigned int uiDPKey, CNetHead& stHead, const char* pszBuf, int iBufLen);

	//��ȡ������svr������Ϣ�ı��뻺����
	CBuffer& GetServerEncodeBuffer()
	{
		return m_stServerEncodeBuf;
	}

	int SendToServer(int iSvrID, const char* pszBuf, int iBufLen);   ///���̨��������

	//��CNetHead���ж��û��Ƿ��ѵ�¼
	bool IsLogined(const CNetHead& stHead);

	void AttachUserToWB(sl::uid_t iUID);
	void DettachUserFromWB(sl::uid_t iUID);

	///CEpollAndShmSvrҪ��ʵ�ֵĺ���
public:
	//����������Ŀ
	virtual int LoadConfig();

	///�ض�������Ŀ
	virtual int ReLoadConfig();

	//����ҵ��ĳ�ʼ��
	virtual int WorkInit();

	///runѭ���е�һ��ִ�й���
	virtual void RunOne();

	virtual void DoExit();

protected:
	int InitAppBuffer();
	
	//���ͳ����Ϣ
	void DumpStatInfo();

	//����˵�����
	void CheckSvrConnect();

    ///����ܵ��е����ݰ�
	void OnRecvData(sl::uint uiPathKey);

	//�����û�����
	void AcceptReq(CBuffer& stBuff, int iLen);

	//��������
	int RecvData(unsigned int uiDPKey, CBuffer& stBuff);

	int SetOfflining(const CNetHead& stHead);

public:
	///����Ƿ����Թ���˿ڵ���Ϣ
	static bool IsFromAdminPort(const CNetHead& stHead);

	//������ĳ�ʼ������
	static int InitCmdFactory(char* pszBuff, unsigned int uiSize, bool bResetShm);

	///�¼��ص�����
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
	bool					m_InitFinish;					///< ��ʼ���Ƿ����

	unsigned int			m_SecondCount;
	unsigned int			m_MinitCount;						///< ���Ӽ���
	unsigned int			m_LastTime;							///< �ϴμ�¼ʱ��

	CBuffer					m_stClientEncodeBuf;				///< Client���뻺����
	CBuffer					m_stClientDecodeBuf;				///< Client���뻺����
	CBuffer					m_stServerEncodeBuf;				///< Server���뻺����

	CStat					m_Stat;								///< ���

	CSvrConnect				m_SvrConnect[EMAX_SVRCONN_COUNT];	///< �������B��

	CShmBuff				m_stShmBuff;						///< �����ڴ����ݻ�����
};

#endif