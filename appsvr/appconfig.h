///app配置类
/********************************************************************
	created:	2015/12/15
	created:	15:12:2015   21:59
	filename: 	d:\workspace\shyloo\appsvr\appconfig.h
	file path:	d:\workspace\shyloo\appsvr
	file base:	appconfig
	file ext:	h
	author:		ddc
	
	purpose:	app配置类
*********************************************************************/

#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include "../sllib/slbase_define.h"
#include "../sllib/sltype.h"
#include "svrconn.h"
#define APP_CONF_PATH	"./config/appsvr.jsn"

namespace sl
{
	class CAppConfig
	{
	public:
		CAppConfig(): ServerID(0), CodeStreamSize(0), LogFileNum(0), LogFileSize(100000){}

		int		ServerID;			///< SvrID
		string  Platform;			///< 平台
		string  SoPath;				///< 业务逻辑so的路径
		vector<ushort> AdminPorts;  ///< admin 端口

		string CmdFactoryConf;		///< 命令工厂的配置串

		///共享内存管道配置
		string	CodeStreamKey;
		int		CodeStreamSize;
		string	CodeFrontEndSocket;
		string  CodeBackEndSocket;

		//日志配置
		string	LogFormat;
		string  LogFilter;
		int		LogFileNum;
		int		LogFileSize;

		int		DBWriteBackTime;

		//svr连接配置
		vector<CSvrConnectParam> SvrConnectInfo;
		
		
	};
}

#define APP_CONF   (sl::CSingleton<CAppConfig>::Instance())
#endif