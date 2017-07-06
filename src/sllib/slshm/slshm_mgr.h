#ifndef __SL_LIB_SHM_MGR_H__
#define __SL_LIB_SHM_MGR_H__
#include <string>
#include <vector>
#include "slpool.h"
#include "slshm_queue.h"
#include "slshm.h"
#include "slsingleton.h"
namespace sl{
namespace shm{
class SLShmMgr : public ISLShmMgr, public CSingleton<SLShmMgr>{
public:
	SLShmMgr() {}
	virtual ~SLShmMgr() {}
	virtual ISLShmQueue* SLAPI createShmQueue(bool bBackEnd, const char* pszShmKey, int sendSize, int recvSize, bool clear = true);
	virtual void SLAPI release(void) { DEL this; }
	virtual void SLAPI recover(ISLShmQueue* queue);

private:
	std::string getShmKeyFile(const char* shmKey);

private:
	sl::SLPool<SLShmQueue> _shmQueues;
};
}
}

#endif
