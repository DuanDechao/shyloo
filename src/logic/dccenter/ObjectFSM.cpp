#include "ObjectFSM.h"
#include "IDCCenter.h"
#include "ObjectMgr.h"
ObjectFSM::ObjectFSM():_status(0){}
ObjectFSM::~ObjectFSM(){}

bool ObjectFSM::EntryStatus(IKernel* pKernel, IObject* object, int32 status, const void* context, const int32 size){
	int64 changeStatus = ((((int64)_status) << 32) | (int64)status);
	if (_status != status){
		if (!_leaveJudgeCBPool.Call(_status, false, pKernel, object, context, size)
			|| !_changeJudgeCBPool.Call(changeStatus, false, pKernel, object, context, size)
			|| !_entryJudgeCBPool.Call(status, false, pKernel, object, context, size)){
			return false;
		}

		_leaveStatusCBPool.Call(_status, pKernel, object, context, size);
		_status = status;
		object->setPropInt32(ObjectMgr::getInstance()->getObjectStatusProp(), status);

		_changeStatusCBPool.Call(changeStatus, pKernel, object, context, size);
		_entryStatusCBPool.Call(status, pKernel, object, context, size);
	}
	else{
		if (!_changeJudgeCBPool.Call(changeStatus, false, pKernel, object, context, size)
			|| !_entryJudgeCBPool.Call(status, false, pKernel, object, context, size)){
			return false;
		}

		_changeStatusCBPool.Call(changeStatus, pKernel, object, context, size);
		_entryStatusCBPool.Call(status, pKernel, object, context, size);
	}
	return true;
}
