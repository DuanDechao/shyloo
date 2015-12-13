//网络连接层启动类
/********************************************************************
	created:	2015/12/03
	created:	3:12:2015   20:06
	filename: 	e:\myproject\shyloo\netsvr\netsvr.cpp
	file path:	e:\myproject\shyloo\netsvr
	file base:	netsvr
	file ext:	cpp
	author:		ddc
	
	purpose:	网络连接层启动类
*********************************************************************/

#include "netdef.h"
#include "netctrl.h"
#include "netconfig.h"
using namespace sl;

#ifndef _DEBUG
#define THIS_BUILD "debug"
#else
#define THIS_BUILD "release"
#endif

void PrintVersion()
{
	printf("%s(%s) version %s start... \n", APPNAME, THIS_BUILD, "1");
	printf("last build time:%s %s\n", __DATE__, __TIME__);
}

void PrintUsage()
{
	PrintVersion();
	printf("Usage: %s [options]\n", APPNAME);
	printf("\nOption:\n");
	printf("%-8s run program not daemon\n", "-g");
	printf("%-8s show help\n", "-h");
	printf("%-8s show version\n", "-v");
	printf("%-8s ConfigFileName\n", "-c");
	printf("\nCommands:\n");
	printf("kill -s USR1 <pid> \t notify %s reload config\n", APPNAME);
	printf("kill -s USR2 <pid> \t notify %s exit\n", APPNAME);

}

void AtExit()
{
	CServerUtils::RemovePidFile(APPNAME);
}


int main(int argc, char** argv)
{
	PrintVersion();
	int iRet = 0;
	bool bDaemon = true;   ///默认后台执行
	bool iOpt = 0;

	//while( (iOpt = getopt(argc, argv, "vghc:")) != -1)
	//{
	//	switch(iOpt)
	//	{
	//	case 'v':
	//		return 0;
	//	case 'g':
	//		bDaemon = false;
	//		break;;
	//	case 'h':
	//		PrintUsage();
	//		return 0;
	//	case 'c':
	//		//指定配置文件名
	//		CONF->ConfigName = optarg;
	//		break;
	//	default:
	//		break;
	//	}
	//}

#ifndef SL_OS_WINDOWS
	///后台运行
	if(bDaemon)
	{
		CServerUtils::InitDaemon();
	}

#endif

#if defined(_DEBUG)
	if(!bDaemon)
	{
		SL_NLOG->AddFormat(EStdOut);
	}
#endif
	// 避免启动多个进程
	if (!CServerUtils::CheckSingleRun(argv[0]))
	{
		return -1;
	}
	atexit(AtExit);

	CNetCtrl ctrl;
	iRet = ctrl.Init();
	iRet = ctrl.Run();

	iRet = ctrl.Exit();
	
	return 0;
}