#ifndef SL_SLSHM_H
#define SL_SLSHM_H
#include "sltype.h"
namespace sl{
namespace shm{
class ISLShmQueue{
public:
	virtual int SLAPI getRecvQueueFreeSize() const = 0;
	virtual int SLAPI getSendQueueFreeSize() const = 0;
	virtual int SLAPI putData(const void* pszBuffer, int len) = 0;
	virtual int SLAPI getData(char* p, int buflen, int& len) = 0;
	virtual int SLAPI remove() = 0;
	virtual bool SLAPI hasCode() const = 0;
	virtual bool SLAPI getDataLen() const = 0;
};

class ISLShmMgr{
public:
	virtual ISLShmQueue* SLAPI createShmQueue(bool bBackEnd, const char* pszShmKey, int iShmSize) = 0;
	virtual void SLAPI release(void) = 0;
	virtual void SLAPI recover(ISLShmQueue* queue) = 0;
};

extern "C" SL_DLL_API sl::shm::ISLShmMgr* SLAPI newShmMgr();

}
}

#endif