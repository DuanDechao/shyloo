#ifndef __SL_FRAMEWORK_RELATION_H__
#define __SL_FRAMEWORK_RELATION_H__
#include "IRelation.h"
#include "slsingleton.h"

class IEventEngine;
class IHarbor;
class IObjectLocator;
class IPacketSender;
class Relation : public IRelation, public sl::SLHolder<Relation>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void sendToClient(int64 actorId, int32 messageId, const sl::OBStream& args);

private:
	Relation*			_self;
	sl::api::IKernel*	_kernel;
	IHarbor*			_harbor;
	IObjectLocator*		_objectLocator;
	IPacketSender*		_packetSender;
};
#endif