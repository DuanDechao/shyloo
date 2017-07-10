#define SL_DLL_EXPORT
#include "slshm_mgr.h"
namespace sl{
SL_SINGLETON_INIT(shm::SLShmMgr);
namespace shm{
ISLShmQueue* SLShmMgr::createShmQueue(bool bBackEnd, const char* pszShmKey, int sendSize, int recvSize, bool clear){
	std::string key = getShmKeyFile(pszShmKey);
	return (ISLShmQueue*)CREATE_FROM_POOL(_shmQueues, bBackEnd, key.c_str(), sendSize, recvSize, clear);
}

void SLShmMgr::recover(ISLShmQueue* queue){
	if (queue)
		_shmQueues.recover((SLShmQueue*)queue);
}

std::string SLShmMgr::getShmKeyFile(const char* shmKey){

	FILE* pstFile = fopen(shmKey, "a+");
	if (pstFile)
		fclose(pstFile);
	else
		SLASSERT(false, "wtf");

	char keyPathStr[128] = { 0 };
	SafeSprintf(keyPathStr, sizeof(keyPathStr), "TEXT(%s)", shmKey);
	return keyPathStr;
}

extern "C" SL_DLL_API ISLShmMgr* SLAPI newShmMgr(void){
	SLShmMgr* g_shmMgr = SLShmMgr::getSingletonPtr();
	if (g_shmMgr == NULL)
		g_shmMgr = NEW SLShmMgr();
	return SLShmMgr::getSingletonPtr();
}
}
}