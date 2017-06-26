#ifndef SL_SLSHM_H
#define SL_SLSHM_H
#include "sltype.h"
namespace sl{
namespace shm{
class ISLShmQueue{
public:
	virtual int SLAPI getRecvQueueFreeSize() const = 0;
	virtual int SLAPI getSendQueueFreeSize() const = 0;
	virtual int SLAPI getRecvQueueDataSize() const = 0;
	virtual int SLAPI getSendQueueDataSize() const = 0;
	virtual int SLAPI putData(const void* pszBuffer, int len) = 0;
	virtual const char* SLAPI getData(char* p, int readLen) = 0;
	virtual const char* SLAPI peekData(char *p, int peekLen) = 0;
	virtual void SLAPI skip(const int len) = 0;
};

class ISLShmMgr{
public:
	virtual ISLShmQueue* SLAPI createShmQueue(bool bBackEnd, const char* pszShmKey, int sendSize, int recvSize, bool clear = true) = 0;
	virtual void SLAPI release(void) = 0;
	virtual void SLAPI recover(ISLShmQueue* queue) = 0;
};

extern "C" SL_DLL_API sl::shm::ISLShmMgr* SLAPI newShmMgr();

}
}

#endif