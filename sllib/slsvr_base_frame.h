///服务器基本框架类
/********************************************************************
	created:	2015/12/01
	created:	1:12:2015   20:10
	filename: 	e:\myproject\shyloo\sllib\slsvr_base_frame.h
	file path:	e:\myproject\shyloo\sllib
	file base:	slsvr_base_frame
	file ext:	h
	author:		ddc
	
	purpose:	服务器基本框架类
*********************************************************************/
#ifndef _SL_SVR_BASE_FRAME_H_
#define _SL_SVR_BASE_FRAME_H_
#include "slsingleton.h"
#include "slconfig.h"
#include <string>

#ifdef SL_OS_WINDOWS
	#define	 fork()		(1)
	#define  signal(signnum, handler) (0)
	#define  getopt(argc, argv, optstring) (0)
	#define optarg ""
	#define SIGUSR1 10
	#define SIGUSR2 12
	#define kill(pid, sig) (0)
	#define WNONHANG 1
	#define WUNTRACED 2
	#define waitpid(pid, status, options) (0)
#else
	#include <sys/types.h>
	#include <signal.h>
	#include <sys/wait.h>
#endif
namespace sl
{
	class CSvrBaseFrame;
	//主控对象管理类，主要用于维持一个唯一的主控对象
	class CSvrCtrl
	{
	private:
		CSvrBaseFrame* m_stCtrl;
	public:
		CSvrCtrl(): m_stCtrl(NULL) {}
		~CSvrCtrl() {}

		int RegisterCtrl(CSvrBaseFrame* pstCtrl)
		{
			if(m_stCtrl)
			{
				return -1;
			}
			m_stCtrl = pstCtrl;
			return 0;
		}
		
		CSvrBaseFrame* GetCtrl() const {return m_stCtrl;}
	};

	//获取主控对象的指针
#define PSVRCTRL	(CSingleton<CSvrCtrl>::Instance())->GetCtrl()
	
	//注册为主控对象的宏
#define REGISTERCTRL(stctrl) \
	do { \
	int iRet = (CSingleton<CSvrCtrl>::Instance())->RegisterCtrl(stctrl); \
	}while(false)
	//服务器基本框架
	class CSvrBaseFrame
	{
	public:
		CSvrBaseFrame(const char* pszAppName) 
		{
			m_bReloadConfig	= false;
			m_bExit			= false;
			if(pszAppName)
			{
				
			}else
			{
				m_strSvrName = "noname svr";
			}
			
		}
		~CSvrBaseFrame() {}
	public:
		int Init()
		{
			int iRet = 0;

			//注册信号量
			iRet = RegisterSig();

			//读取配置
			iRet = LoadConfig();

			///初始化数据通道
			iRet = InitDataStream();

			//执行业务数据初始化
			iRet = WorkInit();

			return iRet;
		}

		int Run()
		{
			while(true)
			{
				if(m_bExit)
				{
					//最先检查程序退出
					m_bExit = false;
					return 0;
				}

				//检查信号
				CheckAndDoneSigEvent();

				//处理数据信号，并等待
				WaitAndEvent();

				//执行循环业务
				RunOne();
			}
			return 0;
		}

		virtual void Exit()
		{
			DoExit();
		}
	public:
		//处理信号函数
		static void Sigusr1Handle(int iSigVal)
		{
			if(PSVRCTRL)
			{
				PSVRCTRL->OnSiguser1Handle();
			}
			signal(SIGUSR1, CSvrBaseFrame::Sigusr1Handle);
		}

		static void Sigusr2Handle(int iSigVal)
		{
			if(PSVRCTRL)
			{
				PSVRCTRL->OnSiguser2Handle();
			}
			signal(SIGUSR2, CSvrBaseFrame::Sigusr2Handle);
		}

		const char* GetSvrName()
		{
			return m_strSvrName.c_str();
		}
	protected:
		//信号量1处理函数
		virtual void OnSiguser1Handle()
		{
			SetReloadFlag(true);
		}

		//信号量2处理函数
		virtual void OnSiguser2Handle()
		{
			SetExitFlag(true);
		}

		void SetReloadFlag(bool bReLoad)
		{
			m_bReloadConfig = bReLoad;
		}

		void SetExitFlag(bool bExit)
		{
			m_bExit = bExit;
		}

		virtual int RegisterSig()
		{
			//安装信号处理函数
			signal(SIGUSR1, CSvrBaseFrame::Sigusr1Handle);    ///< kill -USR1 pid 会让程序重读配置
			signal(SIGUSR2, CSvrBaseFrame::Sigusr2Handle);	  ///< kill -USR1 pid 会让程序安全的退出
		}
		
		///初始化数据通道
		///例如跟其他svr之间共享内存， 跟db之间的数据通路
		virtual int InitDataStream() = 0;

		virtual int LoadConfig()  =  0;     ///< 加载配置项目

		//并不是所有配置都可以reload所以这不设置实现
		virtual int ReLoadConfig() = 0;     ///< 重读配置项目

		///具体业务的初始化
		virtual int WorkInit() = 0;

		//等待处理事件发生或休眠
		virtual void WaitAndEvent() = 0;

		virtual void RunOne() = 0;          ///< run循环中的一次执行过程

		virtual void DoExit() = 0;			///< 退出时要执行的逻辑，例如回写

	private:
		virtual void CheckAndDoneSigEvent() ///< 检查信号量
		{
			if(m_bReloadConfig)
			{
				ReLoadConfig();
				m_bReloadConfig = false;
			}
		}

	protected:
		std::string		m_strSvrName;			///<  服务器名称
		bool		m_bReloadConfig;		///<  重读配置信号
		bool		m_bExit;				///<  退出程序信号
	};
}
#endif