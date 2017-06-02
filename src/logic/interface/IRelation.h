#ifndef __SL_INTERFACE_RELATION_H__
#define __SL_INTERFACE_RELATION_H__
#include "slimodule.h"
#include "slbinary_stream.h"
class IRelation : public sl::api::IModule{
public:
	virtual ~IRelation() {}

	virtual void sendToClient(int64 actorId, int32 messageId, const sl::OBStream& args) = 0;
};
#endif