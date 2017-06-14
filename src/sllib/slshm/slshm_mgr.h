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
	virtual ISLShmQueue* SLAPI createShmQueue(bool bBackEnd, const char* pszShmKey, int iShmSize);
	virtual void SLAPI release(void) { DEL this; }
	virtual void SLAPI recover(ISLShmQueue* queue);

private:
	sl::SLPool<SLShmQueue> _shmQueues;
};
}
}

#endif