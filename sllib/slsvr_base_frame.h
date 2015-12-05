///���������������
/********************************************************************
	created:	2015/12/01
	created:	1:12:2015   20:10
	filename: 	e:\myproject\shyloo\sllib\slsvr_base_frame.h
	file path:	e:\myproject\shyloo\sllib
	file base:	slsvr_base_frame
	file ext:	h
	author:		ddc
	
	purpose:	���������������
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
	//���ض�������࣬��Ҫ����ά��һ��Ψһ�����ض���
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

	//��ȡ���ض����ָ��
#define PSVRCTRL	(CSingleton<CSvrCtrl>::Instance())->GetCtrl()
	
	//ע��Ϊ���ض���ĺ�
#define REGISTERCTRL(stctrl) \
	do { \
	int iRet = (CSingleton<CSvrCtrl>::Instance())->RegisterCtrl(stctrl); \
	}while(false)
	//�������������
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

			//ע���ź���
			iRet = RegisterSig();

			//��ȡ����
			iRet = LoadConfig();

			///��ʼ������ͨ��
			iRet = InitDataStream();

			//ִ��ҵ�����ݳ�ʼ��
			iRet = WorkInit();

			return iRet;
		}

		int Run()
		{
			while(true)
			{
				if(m_bExit)
				{
					//���ȼ������˳�
					m_bExit = false;
					return 0;
				}

				//����ź�
				CheckAndDoneSigEvent();

				//���������źţ����ȴ�
				WaitAndEvent();

				//ִ��ѭ��ҵ��
				RunOne();
			}
			return 0;
		}

		virtual void Exit()
		{
			DoExit();
		}
	public:
		//�����źź���
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
		//�ź���1������
		virtual void OnSiguser1Handle()
		{
			SetReloadFlag(true);
		}

		//�ź���2������
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
			//��װ�źŴ�����
			signal(SIGUSR1, CSvrBaseFrame::Sigusr1Handle);    ///< kill -USR1 pid ���ó����ض�����
			signal(SIGUSR2, CSvrBaseFrame::Sigusr2Handle);	  ///< kill -USR1 pid ���ó���ȫ���˳�
		}
		
		///��ʼ������ͨ��
		///���������svr֮�乲���ڴ棬 ��db֮�������ͨ·
		virtual int InitDataStream() = 0;

		virtual int LoadConfig()  =  0;     ///< ����������Ŀ

		//�������������ö�����reload�����ⲻ����ʵ��
		virtual int ReLoadConfig() = 0;     ///< �ض�������Ŀ

		///����ҵ��ĳ�ʼ��
		virtual int WorkInit() = 0;

		//�ȴ������¼�����������
		virtual void WaitAndEvent() = 0;

		virtual void RunOne() = 0;          ///< runѭ���е�һ��ִ�й���

		virtual void DoExit() = 0;			///< �˳�ʱҪִ�е��߼��������д

	private:
		virtual void CheckAndDoneSigEvent() ///< ����ź���
		{
			if(m_bReloadConfig)
			{
				ReLoadConfig();
				m_bReloadConfig = false;
			}
		}

	protected:
		std::string		m_strSvrName;			///<  ����������
		bool		m_bReloadConfig;		///<  �ض������ź�
		bool		m_bExit;				///<  �˳������ź�
	};
}
#endif