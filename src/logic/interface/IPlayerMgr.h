#ifndef _SL_INTERFACE_PLAYERMGR_H__
#define _SL_INTERFACE_PLAYERMGR_H__
#include "slimodule.h"
#include <functional>
#include "slbinary_stream.h"
class IObject;
class IPlayerMgr : public sl::api::IModule{
public:
	virtual ~IPlayerMgr() {}

	virtual bool active(int64 actorId, int32 nodeId, int64 accountId, const std::function<void(sl::api::IKernel* pKernel, IObject* object, bool isReconnect)>& f) = 0;
	virtual bool deActive(int64 actorId, int32 nodeId, bool isPlayerOpt) = 0;
};

#endif