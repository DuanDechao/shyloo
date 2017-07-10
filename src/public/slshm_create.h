//共享内存的封装类
#ifndef _SL_PUBLIC_SHM_CREATE_H_
#define _SL_PUBLIC_SHM_CREATE_H_
#include "slmulti_sys.h"

//共享内存
#ifdef SL_OS_WINDOWS
#include <Windows.h>
#include <string>
namespace sl{
class SLShm{
public:
	SLShm() : _key(""), _size(0), _shm(0), _pszBuf(0){}
	
	~SLShm(){
		close();
	}

	//创建或连接上一块共享内存
	int32 create(const char* pszKey, size_t sSize){
		SLASSERT(pszKey != NULL, "wtf");

		_key.assign(pszKey);
		_size	=	sSize;
		_shm	=	CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, (DWORD)_size, _key.c_str());
		
		if(_shm && (errno == ERROR_ALREADY_EXISTS)){
			CloseHandle(_shm);
			_shm  =   OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, _key.c_str());
			if(!_shm){
				SLASSERT(false, "OpenFileMapping: %d", SL_ERRNO);
				return -1;
			}
		}
		
		if(_shm){	
			_pszBuf = (char*) MapViewOfFile(_shm, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if(!_pszBuf){
				SLASSERT(false, "MapViewOfFile: %d, Handle=%x", SL_ERRNO, _shm);
				CloseHandle(_shm);
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
	int32 close(){
		if(_pszBuf){
			if(!UnmapViewOfFile(_pszBuf)){
				SLASSERT(false, "remove shm failed");
			}
			_pszBuf = NULL;
		}
		if(_shm){
			if(!CloseHandle(_shm)){
				SLASSERT(false, "close shm handle failed");
			}
			_shm = NULL;
		}
		return 0;
	}

	inline size_t getSize() const {return _size;}
	inline char* getBuffer() const {return _pszBuf;}

protected:
	std::string		_key;
	size_t			_size;

	HANDLE			_shm;
	char*			_pszBuf;

};
}
#else
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
namespace sl{
class SLShm{
public:
	SLShm(): _key(0), _size(0), _shm(0), _pszBuf(0){}

	//退出时不关闭共享内存
	~SLShm(){}

	//创建或连接上一块共享内存
	int32 create(key_t uiKey, size_t sSize){
		_key		=	(uint32) uiKey;
		_size		=	sSize;

		_shm		=	shmget(uiKey, _size, IPC_CREAT|IPC_EXCL|0666);
		if(_shm < 0){
			if( SL_ERRNO != EEXIST){
				ECHO_ERROR("shmget create fail: errno=%d, key=%u, size=%u", SL_ERRNO, uiKey, _size);
				return -1;
			}

			_shm = shmget(uiKey, _size, 0666);
			if(_shm < 0){
				_shm = shmget(uiKey, 0, 0666);
				if(_shm < 0){
					ECHO_ERROR("shmget 0: errno=%d, key=%u", SL_ERRNO, uiKey);
					return -2;
				}

				if(shmctl(_shm, IPC_RMID, 0)){
					ECHO_ERROR("shmctl %d fail: errno=%d", _shm, SL_ERRNO);
					return -3;
				}
				_shm = shmget(uiKey, _size, IPC_CREAT|IPC_EXCL|0666);
				if(_shm < 0){
					ECHO_ERROR("shmget recreate fail: errno=%d, key=%u, size=%u", SL_ERRNO, uiKey, _size);
					return -4;
				}
			}
			ECHO_TRACE("attach shm ok, id = %d", _shm);
		}
		else{
			ECHO_TRACE("create shm ok, id = %d", _shm);
		}

		_pszBuf  = (char*) shmat(_shm, 0, 0);
		if(!_pszBuf){
			ECHO_ERROR("shmat %d fail: %d", _shm, SL_ERRNO);
			return -5;
		}

		return 0;
	}

	//根据文件计算用于创建共享内存的key
	static key_t FtoK(const char* pszPathName){
		key_t key;
		if((key = ftok(pszPathName,'S')) < 0){
			return SL_ERRNO;
		}
		return key;
	}

	//创建或连接上一块共享内存
	//pszPathName是ftok的参数，pszPathName对应的文件必须要存在
	int32 create(const char* pszPathName, size_t sSize){
		key_t iKey = FtoK(pszPathName);
		ECHO_TRACE("FtoK(%s) key=0x%x(%d)", pszPathName, iKey, iKey);
		if(iKey < 0){
			ECHO_ERROR("ftok(%s) failed %d", pszPathName, iKey);
			return -1;
		}
		return create(iKey, sSize);
	}

	//关闭一块共享内存
	int32 close(){
		if(_shm > 0){
			if(shmctl(_shm, IPC_RMID, 0)){
				ECHO_ERROR("shmctl %d: %d", _shm, SL_ERRNO);
				return -1;
			}
		}
		return 0;
	}

	char* getBuffer() const {return _pszBuf;}
	key_t getKey() const {return _key;}
	size_t getSize() const {return _size;}

protected:
	uint32			_key;
	size_t			_size;
	int32			_shm;
	char*			_pszBuf;
}; //class CShm
}
#endif

#endif