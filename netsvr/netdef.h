//net全局定义类
/********************************************************************
	created:	2015/12/02
	created:	2:12:2015   16:12
	filename: 	e:\myproject\shyloo\netsvr\netdef.h
	file path:	e:\myproject\shyloo\netsvr
	file base:	netdef
	file ext:	h
	author:		ddc
	
	purpose:	net全局定义类
*********************************************************************/

#ifndef _NET_DEF_H_
#define _NET_DEF_H_
#include "../sllib/slbase.h"
#include "../sllib/slconfig.h"
#include "../sllib/slbuffer.h"
#include "../sllib/slobject_mgr.h"
#include "../sllib/slmsg_base.h"
#include "../sllib/net/slepoll.h"
#include "../sllib/net/slsocket.h"
#include "../sllib/slarray.h"
#include "../sllib/slshm_queue.h"
#include "../sllib/slserver_utils.h"
#include "../sllib/slsize_string.h"
#include "../sllib/slsvr_base_frame.h"
#include "../sllib/slini_config.h"


//==========================================================
// 相关文件的定义，作为一个约定，不用再配置文件中读取
#define APPNAME				"netsvr"
#define FILE_CONFIG			"./config/netsvr.conf"
#define FILE_NORMALLOG		"./log/netsvr.log"
#define FILE_ERRORLOG		"./log/netsvr.err"
#define FILE_STATLOG		"./log/netsvr.sta"
#define FILE_PIDFILE        "./netsvr.pid"


#define CHECK_RETURN(iRet)  do{if(iRet) {return iRet;}} while(0)
enum net_const
{
	NET_MAX_LISTEN		=		8,			///< 最多能侦听NET_MAX_LISTEN个端口
	NET_WAIT_SIZE		=		1024,
	NET_FILE_LENGTH		=		4096,
};

namespace sl
{
	class CNetListenInfo
	{
	public:
		CNetListenInfo()
		{
			
		}
	public:
		CSizeString<64>			m_szListenIP;			///< 侦听的IP
		unsigned short			m_unListenPort;			///< 侦听的端口
		unsigned int			m_uiIdleTimeOut;		///< 发呆超时值，默认为-1，即不检测发呆，accept的sock会继承此值
		enumNetPackage			m_stPackageType;		///< 数据包的格式
		unsigned char			m_ucEncodeMethod;		///< 消息体编码方法
		unsigned char			m_ucDecodeMethod;		///< 消息体解码方法

	};

	//=====================================================
	//
#ifdef WIN32
#define signal(signum, handler) (0)
#define getopt(argc, argv, optstring) (0)

#else
#include <sys/types.h>
#include <signal.h>
#endif
}
#endif