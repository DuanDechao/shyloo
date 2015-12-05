///server开发的一些常用函数
/********************************************************************
	created:	2015/12/03
	created:	3:12:2015   20:15
	filename: 	e:\myproject\shyloo\sllib\slserver_utils.h
	file path:	e:\myproject\shyloo\sllib
	file base:	slserver_utils
	file ext:	h
	author:		ddc
	
	purpose:	server开发的一些常用函数
*********************************************************************/
#ifndef _SL_SERVER_UTILS_H_
#define _SL_SERVER_UTILS_H_
#include "../sllib/slconfig.h"
#ifndef SL_OS_WINDOWS
	#include <signal.h>
#endif
namespace sl
{
	///封装了Server开发的一些常用函数
	class CServerUtils
	{
	public:
		///转为守护进程
		static void InitDaemon(void)
		{
			pid_t pid;
			if((pid = fork()) != 0)
			{
				exit(0);
			}
			
			///setsid();

			signal(SIGHUP,	SIG_IGN);		///< Term 终端的挂断或进程死亡
			signal(SIGINT,	SIG_IGN);		///< Term 来自键盘的中断信号
			signal(SIGQUIT, SIG_IGN);		///< Core 来自键盘的离开信号
			signal(SIGPIPE, SIG_IGN);		///< Term 管道损坏：向一个没有读进程的管道写数据
			signal(SIGTTOU, SIG_IGN);		///< Stop 后台进程企图从控制终端写
			signal(SIGTTIN, SIG_IGN);		///< Stop 后台进程企图从控制终端读
			signal(SIGCHLD, SIG_IGN);		///< IGN  子进程停止或终止
			signal(SIGTERM, SIG_IGN);		///< Term 终止
			signal(SIGUSR1, SIG_IGN);
			signal(SIGUSR2, SIG_IGN);

			//Ignore(SIGPIPE);
			//Ignore(SIGHUP);

			if((pid = fork()) != 0)
			{
				exit(0);
			}

			umask(0);		///< 默认开放所有的权限

			

		}


	}; //class CServerUtils

}
#endif