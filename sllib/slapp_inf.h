//appsvr提供给业务逻辑so的使用的接口类
/********************************************************************
	created:	2015/12/15
	created:	15:12:2015   20:50
	filename: 	d:\workspace\shyloo\sllib\slapp_inf.h
	file path:	d:\workspace\shyloo\sllib
	file base:	slapp_inf
	file ext:	h
	author:		ddc
	
	purpose:	appsvr提供给业务逻辑so的使用的接口类
*********************************************************************/
#ifndef __SL_APP_INF_H__
#define __SL_APP_INF_H__
#include "slassert.h"
#include "slasync_cmd_factory.h"
#include "slmsg_base.h"
#include "ipc/slshm_buff.h"

namespace sl
{
	///一个初始为NULL的指针
	namespace detail
	{
		class CVoidPtr
		{
		public:
			CVoidPtr(): p(NULL){}
			void* p;
		};
	}

	//AppSo的单件
	#define SL_APP_RAW	(CSingleton<detail::CVoidPtr, 0>::Instance()->p)
	#define SL_APP ((CAppInf*)SL_APP_RAW)

	//断线的原因
	enum EnumUserDisconnectReason
	{
		EUDR_UNKNOW		=	0,		///< 其他原因，主要是非正常原因
		EUDR_KICK		=	1,		///< 被踢掉。重复登录
		EUDR_SVR_HALT	=	2,		///< Svr停机
		EUDR_MULT_LOGIN =   3,		///< 多次登录 在已经登录的情况下又发送了登录协议
		EUDR_LOGIN_FAIL =	4,		///< 登录失败
		EUDR_FROZEEN	=	5,		///< 封号
		EUDR_OFFLINE	=	6,		///< 用户下线
		EUSR_RECONN		=	7,		///< 断线重连时要把以前的NetHead清除掉
	};

	//AppSvr会从这里继承，为so提供访问AppSvr的能力
	class CAppInf
	{
	public:
		CAppInf()
		{
			//在CAppSvr初始化时，把CAppSvr对象赋给SL_APP
			SL_APP_RAW = this;
		}
		virtual ~CAppInf() {}

		/*
			内存预分配
			@param [in] szBuffName   buffer名称
			@param [in] uiBuffSize	 buffer大小
			@param [in] pInitFunc	 初始化buffer的函数
		*/
		virtual int RegisterBuffer(const char* szBuffName, unsigned int uiBuffSize, PInitBuffFunc pInitFunc) = 0;
		
		//获取向前台发消息的编码缓冲区
		virtual CBuffer& GetClientEncodeBuffer() = 0;
		virtual int SendData(unsigned int uiDPKey, CNetHead& stHead, const char* pszBuf, int iBufLen) = 0;

		///获取向其他svr发消息的编码缓冲区
		virtual CBuffer& GetServerEncodeBuffer() = 0;
		virtual int SendToServer(int iSvrID, const char* pszBuf, int iBufLen) = 0;

		// 从CNetHead来判断用户是否登录
		virtual bool IsLogined(const CNetHead& stHead) = 0;

		//定时回写的2个操作
		virtual void AttachUserToWB(sl::uid_t iUID) = 0;
		virtual void DettachUserFromWB(sl::uid_t iUID) = 0;
	};
}
#endif