///app������
/********************************************************************
	created:	2015/12/15
	created:	15:12:2015   21:59
	filename: 	d:\workspace\shyloo\appsvr\appconfig.h
	file path:	d:\workspace\shyloo\appsvr
	file base:	appconfig
	file ext:	h
	author:		ddc
	
	purpose:	app������
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
		string  Platform;			///< ƽ̨
		string  SoPath;				///< ҵ���߼�so��·��
		vector<ushort> AdminPorts;  ///< admin �˿�

		string CmdFactoryConf;		///< ����������ô�

		///�����ڴ�ܵ�����
		string	CodeStreamKey;
		int		CodeStreamSize;
		string	CodeFrontEndSocket;
		string  CodeBackEndSocket;

		//��־����
		string	LogFormat;
		string  LogFilter;
		int		LogFileNum;
		int		LogFileSize;

		int		DBWriteBackTime;

		//svr��������
		vector<CSvrConnectParam> SvrConnectInfo;
		
		
	};
}

#define APP_CONF   (sl::CSingleton<CAppConfig>::Instance())
#endif