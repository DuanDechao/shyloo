//共享内存的封装类
/********************************************************************
	created:	2015/11/20
	created:	20:11:2015   15:18
	filename: 	e:\myproject\shyloo\sllib\ipc\slshm.h
	file path:	e:\myproject\shyloo\sllib\ipc
	file base:	slshm
	file ext:	h
	author:		ddc
	
	purpose:	共享内存的封装类
*********************************************************************/

#ifndef _SL_LIB_SHM_H_
#define _SL_LIB_SHM_H_
#include "slmulti_sys.h"

//共享内存
#ifdef SL_OS_WINDOWS
#include <Windows.h>
#include <string>
namespace sl{
namespace shm{

class SLShm{
public:
	SLShm() : m_sKey(""), m_uiSize(0), m_hShm(0), m_pszBuf(0){}
	
	~SLShm(){
		Close();
	}

	//创建或连接上一块共享内存
	int32 Create(const char* pszKey, size_t sSize){
		SLASSERT(pszKey != NULL, "wtf");

		m_sKey.assign(pszKey);
		m_uiSize	=	sSize;
		m_hShm		=	CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, m_uiSize, m_sKey.c_str());
		
		if(m_hShm && (errno == ERROR_ALREADY_EXISTS)){
			CloseHandle(m_hShm);
			m_hShm  =   OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, m_sKey.c_str());
			if(!m_hShm){
				SLASSERT(false, "OpenFileMapping: %d", SL_ERRNO);
				return -1;
			}
		}
		
		if(m_hShm){	
			m_pszBuf = (char*) MapViewOfFile(m_hShm, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if(!m_pszBuf){
				SLASSERT(false, "MapViewOfFile: %d, Handle=%x", SL_ERRNO, m_hShm);
				CloseHandle(m_hShm);
				return -1;
			}
			return 0;
		}
		else{
			SLASSERT(false, "CreateFileMapping: %d", SL_ERRNO);
			return -1;
		}
		return 0;
	}

	//关闭一块共享内存
	int Close(){
		if(m_pszBuf){
			if(!UnmapViewOfFile(m_pszBuf)){
				SLASSERT(false, "remove shm failed");
			}
			m_pszBuf = NULL;
		}
		if(m_hShm){
			if(!CloseHandle(m_hShm)){
				SLASSERT(false, "close shm handle failed");
			}
			m_hShm = NULL;
		}
		return 0;
	}

	size_t GetSize() const {return m_uiSize;}
	char* GetBuffer() const {return m_pszBuf;}

protected:
	std::string		m_sKey;
	size_t		m_uiSize;

	HANDLE		m_hShm;
	char*		m_pszBuf;

};
}
}
#else
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
namespace sl
{
namespace shm{
class SLShm
{
public:
	SLShm(): m_uiKey(0), m_uiSize(0), m_iShm(0), m_pszBuf(0){}

	//退出时不关闭共享内存
	~SLShm(){}

	//创建或连接上一块共享内存
	int Create(key_t uiKey, size_t sSize)
	{
		m_uiKey		=	(unsigned int) uiKey;
		m_uiSize	=	sSize;

		m_iShm		=	shmget(uiKey, m_uiSize, IPC_CREAT|IPC_EXCL|0666);
		if(m_iShm < 0)
		{
			if( SL_ERRNO != EEXIST)
			{
				perror("shmget error");
				SL_ERROR("shmget create fail: errno=%d, key=%u, size=%u", SL_ERRNO, uiKey, m_uiSize);
				return -1;
			}

			m_iShm = shmget(uiKey, m_uiSize, 0666);
			if(m_iShm < 0)
			{
				m_iShm = shmget(uiKey, 0, 0666);
				if(m_iShm < 0)
				{
					SL_ERROR("shmget 0: errno=%d, key=%u", SL_ERRNO, uiKey);
					return -2;
				}

				if(shmctl(m_iShm, IPC_RMID, 0))
				{
					SL_ERROR("shmctl %d fail: errno=%d", m_iShm, SL_ERRNO);
					return -3;
				}
				m_iShm = shmget(uiKey, m_uiSize, IPC_CREAT|IPC_EXCL|0666);
				if(m_iShm < 0)
				{
					SL_ERROR("shmget recreate fail: errno=%d, key=%u, size=%u", SL_ERRNO, uiKey, m_uiSize);
					return -4;
				}
			}
			SL_TRACE("attach shm ok, id = %d", m_iShm);
		}
		else
		{
			SL_TRACE("create shm ok, id = %d", m_iShm);
		}

		m_pszBuf  = (char*) shmat(m_iShm, 0, 0);
		if(!m_pszBuf)
		{
			SL_ERROR("shmat %d fail: %d", m_iShm, SL_ERRNO);
			return -5;
		}

		return 0;
	}

	//根据文件计算用于创建共享内存的key
	static key_t FtoK(const char* pszPathName)
	{
		key_t key;
		if((key = ftok(pszPathName,'S')) < 0)
		{
			return SL_ERRNO;
		}
		return key;
	}

	//创建或连接上一块共享内存
	//pszPathName是ftok的参数，pszPathName对应的文件必须要存在
	int Create(const char* pszPathName, size_t sSize)
	{
		key_t iKey = FtoK(pszPathName);
		SL_INFO("FtoK(%s) key=0x%x(%d)", pszPathName, iKey, iKey);
		if(iKey < 0)
		{
			SL_ERROR("ftok(%s) failed %d", pszPathName, iKey);
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
				SL_ERROR("shmctl %d: %d", m_iShm, SL_ERRNO);
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
}
#endif

#endif