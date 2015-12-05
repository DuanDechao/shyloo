///server������һЩ���ú���
/********************************************************************
	created:	2015/12/03
	created:	3:12:2015   20:15
	filename: 	e:\myproject\shyloo\sllib\slserver_utils.h
	file path:	e:\myproject\shyloo\sllib
	file base:	slserver_utils
	file ext:	h
	author:		ddc
	
	purpose:	server������һЩ���ú���
*********************************************************************/
#ifndef _SL_SERVER_UTILS_H_
#define _SL_SERVER_UTILS_H_
#include "../sllib/slconfig.h"
#ifndef SL_OS_WINDOWS
	#include <signal.h>
#endif
namespace sl
{
	///��װ��Server������һЩ���ú���
	class CServerUtils
	{
	public:
		///תΪ�ػ�����
		static void InitDaemon(void)
		{
			pid_t pid;
			if((pid = fork()) != 0)
			{
				exit(0);
			}
			
			///setsid();

			signal(SIGHUP,	SIG_IGN);		///< Term �ն˵ĹҶϻ��������
			signal(SIGINT,	SIG_IGN);		///< Term ���Լ��̵��ж��ź�
			signal(SIGQUIT, SIG_IGN);		///< Core ���Լ��̵��뿪�ź�
			signal(SIGPIPE, SIG_IGN);		///< Term �ܵ��𻵣���һ��û�ж����̵Ĺܵ�д����
			signal(SIGTTOU, SIG_IGN);		///< Stop ��̨������ͼ�ӿ����ն�д
			signal(SIGTTIN, SIG_IGN);		///< Stop ��̨������ͼ�ӿ����ն˶�
			signal(SIGCHLD, SIG_IGN);		///< IGN  �ӽ���ֹͣ����ֹ
			signal(SIGTERM, SIG_IGN);		///< Term ��ֹ
			signal(SIGUSR1, SIG_IGN);
			signal(SIGUSR2, SIG_IGN);

			//Ignore(SIGPIPE);
			//Ignore(SIGHUP);

			if((pid = fork()) != 0)
			{
				exit(0);
			}

			umask(0);		///< Ĭ�Ͽ������е�Ȩ��

			

		}


	}; //class CServerUtils

}
#endif