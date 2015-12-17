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
#include "slconfig.h"
#include "slfile_utils.h"
#include <string>
using namespace std;
#ifndef SL_OS_WINDOWS
	#include <signal.h>
#endif
namespace sl
{
	///封装了Server开发的一些常用函数
	class CServerUtils
	{
	public:
#ifndef SL_OS_WINDOWS
		static void Ignore(int signum)
		{
			struct sigaction sig;
			sig.sa_handler = SIG_IGN;
			sig.sa_flags = 0;

			sigemptyset(&sig.sa_mask); //clean signal set
			sigaction(signum, &sig, 0);//signal
		}

		

		///转为守护进程
		static void InitDaemon(void)
		{
			pid_t pid;
			if((pid = fork()) != 0)
			{
				exit(0);
			}
			
			setsid();

			signal(SIGHUP,	SIG_IGN);		///< Term 终端的挂断或进程死亡
			signal(SIGINT,	SIG_IGN);		///< Term 来自键盘的中断信号
			signal(SIGQUIT, SIG_IGN);		///< Core 来自键盘的离开信号
			signal(SIGPIPE, SIG_IGN);		///< Term 管道损坏：向一个没有读进程的管道写数据
			signal(SIGTTOU, SIG_IGN);		///< Stop 后台进程企图从控制终端写
			signal(SIGTTIN, SIG_IGN);		///< Stop 后台进程企图从控制终端读
			signal(SIGCHLD, SIG_IGN);		///< IGN  子进程停止或终止
			signal(SIGTERM, SIG_IGN);		///< Term 终止
			//signal(SIGUSR1, SIG_IGN);
			//signal(SIGUSR2, SIG_IGN);

			Ignore(SIGPIPE);
			Ignore(SIGHUP);

			if((pid = fork()) != 0)
			{
				exit(0);
			}

			umask(0);		///< 默认开放所有的权限
		}
#endif

		/**
        * 避免运行多个实例
        */
#ifndef SL_OS_WINDOWS
        static bool CheckSingleRun(const char* pszAppName)
        {

            bool bRet = false;
            string s;
            string sPidFile;
#if 1
            do
            {
                // 只要文件名，去掉前面的路径
                s = pszAppName;
                size_t iPos = s.find_last_of('/');
                if (iPos != string::npos)
                {
                    s = s.substr(iPos + 1);
                }

                // 读取进程ID
                sPidFile = "./" + s + ".pid";
                CFileUtils::ReadFile(sPidFile.c_str(), s);
                CStringUtils::TrimLeft(s, " \r\n\t");
                CStringUtils::TrimRight(s, " \r\n\t");
                int iPid = CStringUtils::StrToInt<int>(s.c_str());
                if (iPid <= 0)
                {
                    bRet = true;
                    break;
                }

                // 看看pid文件中保存的进程是否存在
                s.assign(pszAppName);
                iPos = s.find_last_of('/');
                if (iPos != string::npos)
                {
                    s = s.substr(iPos + 1);
                }

                string sCmd;
                CStringUtils::Format(sCmd, "ps -p %d | grep %s -c", iPid, s.c_str());
                FILE* pstPipe = popen(sCmd.c_str(), "r");
                if (pstPipe == NULL)
                {
                    printf("popen(%s) fail\n", sCmd.c_str());
                    bRet = false;
                    break;
                }

                char szBuf[32];
                fgets(szBuf, sizeof(szBuf) - 1, pstPipe);
                pclose(pstPipe);

                int iCount = atoi(szBuf);

                // 进程已经存在了
                if (iCount > 0)
                {
                    printf("%s already running!\n", pszAppName);
                    bRet = false;
                    break;
                }

                bRet = true;
            }
            while(0);

            if (bRet)
            {
                CStringUtils::Format(s, "%d\n", getpid());
                int iRet = CFileUtils::WriteFile(sPidFile.c_str(), s);
                if (iRet)
                {
                    printf("cannot write pid: ret=%d errno=%d\n", iRet, SL_ERRNO);
                    return false;
                }

                return true;
            }

            return bRet;
#else
			///方法二 来自UNP
			s = pszAppName;
			size_t iPos = s.find_first_of('/');
			if(iPos != string::npos)
			{
				s = s.substr(iPos + 1);
			}
			sPidFile = "/var/run/"+ s + ".pid";
			
			int			fd;
			char		buf[16];
			fd = open(sPidFile, O_RDWR|O_CREAT, LOCKMODE);
			if(fd < 0)
			{
				exit(1);
			}
			if(lockfile(fd) < 0)
			{
				if(errno == EACCES ||  errno == EAGAIN)
				{
					close(fd);
					printf("%s already running!\n", pszAppName);
					return false;
				}
				exit(1);
			}
			ftruncate(fd, 0);
			sprintf(buf, "%ld", (long)getpid());
			write(fd, buf, strlen(buf)+1);
			return true;
#endif
        }
#else
        static bool CheckSingleRun(const char* pszAppName)
        {
            return true;
        }
#endif


		///删除pid文件
		static void RemovePidFile(const char* pszAppName)
		{
			//只要文件名，去掉前面的路径
			string s(pszAppName);
			size_t iPos = s.find_last_of('/');
			if(iPos != string::npos)
			{
				s = s.substr(iPos + 1);
			}

			//读取进程ID
			string sPidFile = "./" + s +".pid";
			unlink(sPidFile.c_str());

		}


	}; //class CServerUtils

}
#endif