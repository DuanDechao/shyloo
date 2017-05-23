#ifndef _SL_INTERFACE_AOI_H__
#define _SL_INTERFACE_AOI_H__
#include "slimodule.h"
#include "slbinary_stream.h"

class IObject;
class IAOI : public sl::api::IModule{
public:
	virtual ~IAOI() {}

	virtual void broadcast(IObject* object, int32 messageId, const sl::OBStream& args, bool self = false) = 0;
	virtual void foreachNeighbor(IObject* object, const std::function<void(sl::api::IKernel* pKernel, IObject* object)>& f) = 0;
	virtual bool isNeighbor(IObject* object, const int64 id) = 0;
};

#endif