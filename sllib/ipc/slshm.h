//共享内存

#ifndef _SL_SHM_H_
#define _SL_SHM_H_
#include "../slconfig.h"

//共享内存
#ifdef SL_OS_WINDOWS
	#include <Windows.h>
	#include <string>
	namespace sl
	{
		class CShm
		{
		public:
			CShm():	m_sKey(0), m_uiSize(0), m_hShm(0), m_pszBuf(0){}
			~CShm()
			{
				Close();
			}
			//创建或连接上一块共享内存
			int Create(const char* pszKey, size_t sSize)
			{
				SL_ASSERT(pszKey != NULL);

				m_sKey.assign(pszKey);
				m_uiSize	=	sSize;
				m_hShm		=	CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, m_uiSize, m_sKey.c_str());
				if(m_hShm && (errno == ERROR_ALREADY_EXISTS))
				{
					CloseHandle(m_hShm);
					m_hShm  =   OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, m_sKey.c_str());
					if(!m_hShm)
					{
						return -1;
					}
				}
				if(m_hShm)
				{	
					m_pszBuf = (char*) MapViewOfFile(m_hShm, FILE_MAP_ALL_ACCESS, 0, 0, 0);
					if(!m_pszBuf)
					{
						CloseHandle(m_hShm);
						return -1;
					}
					return 0;
				}
				else
				{
					return -1;
				}
				return 0;
			}

			//关闭一块共享内存
			int Close()
			{
				if(m_pszBuf)
				{
					if(!UnmapViewOfFile(m_pszBuf))
					{

					}
					m_pszBuf = NULL;
				}
				if(m_hShm)
				{
					if(!CloseHandle(m_hShm))
					{

					}
					m_hShm = NULL;
				}
				return 0;
			}

			size_t GetSize() const {return m_uiSize;}
			char* GetBuffer() const {return m_pszBuf;}
		protected:
			string		m_sKey;
			size_t		m_uiSize;

			HANDLE		m_hShm;
			char*		m_pszBuf;

		};
	}
#else
	#include <sys/ipc.h>
	#include <sys/shm.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
	namespace sl
	{
		class CShm
		{
		public:
			CShm(): m_uiKey(0), m_uiSize(0), m_iShm(0), m_pszBuf(0){}

			//退出时不关闭共享内存
			~CShm(){}

			//创建或连接上一块共享内存
			int Create(key_t uiKey, size_t sSize)
			{
				m_uiKey		=	(unsigned int) uiKey;
				m_uiSize	=	sSize;

				m_iShm		=	shmget(uiKey, m_uiSize, IPC_CREAT|IPC_EXCL|0666);
				if(m_iShm < 0)
				{
					if(errno != EEXIST)
					{
						return -1;
					}

					m_iShm = shmget(uiKey, m_uiSize, 0666);
					if(m_iShm < 0)
					{
						m_iShm = shmget(uiKey, 0, 0666);
						if(m_iShm < 0)
						{
							return -2;
						}

						if(shmctl(m_iShm, IPC_RMID, 0))
						{
							return -3;
						}
						m_iShm = shmget(uiKey, m_uiSize, IPC_CREAT|IPC_EXCL|0666);
						if(m_iShm < 0)
						{
							return -4;
						}
					}
				}
				else
				{

				}
				m_pszBuf  = (char*) shmat(m_iShm, 0, 0);
				if(!m_pszBuf)
				{
					return -5;
				}

				return 0;
			}

			//根据文件计算用于创建共享内存的key
			static key_t FtoK(const char* pszPathName)
			{
				struct stat stStat;
				memset(&stStat, 0, sizeof(stStat));
				if(stat(pszPathName, &stStat) != 0)
				{
					return -abs(errno);
				}
				return stStat.st_ino;
			}

			//创建或连接上一块共享内存
			//pszPathName是ftok的参数，pszPathName对应的文件必须要存在
			int Create(const char* pszPathName, size_t sSize)
			{
				key_t iKey = FtoK(pszPathName);
				if(iKey < 0)
				{
					return -1;
				}
				return Create(iKey, sSize);
			}

			//关闭一块共享内存
			int Close()
			{
				if(m_iShm > 0)
				{
					if(shmctl(m_iShm, IPC_RMID, 0))
					{
						return -1;
					}
				}
				return 0;
			}

			char* GetBuffer() const {return m_pszBuf;}
			key_t GetKey() const {return m_uiKey;}
			size_t GetSize() const {return m_uiSize;}

		protected:
			unsigned int	m_uiKey;
			size_t			m_uiSize;
			int				m_iShm;
			char*			m_pszBuf;
		}; //class CShm
	}
#endif

#endif