#ifndef _SL_FRAMEWORK_PROPDELAYSENDER_H_
#define _SL_FRAMEWORK_PROPDELAYSENDER_H_
#include "IPropDelaySender.h"
#include "slsingleton.h"

class IHarbor;
class IEventEngine;
class IProp;
class IObject;
class IPacketSender;
class PropDelaySender : public IPropDelaySender, public sl::SLHolder<PropDelaySender>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void syncChangedProps(IObject* object);
	virtual void removeChangedProp(IObject* object, const IProp* prop);

	void onPlayerOnline(sl::api::IKernel* pKernel, const void* context, const int32 size);
	void onPlayerReconnect(sl::api::IKernel* pKernel, const void* context, const int32 size);

	void syncToSelf(sl::api::IKernel* pKernel, IObject* object, const char* name, const IProp* prop, const bool sync);
	void syncToOthers(sl::api::IKernel* pKernel, IObject* object, const char* name, const IProp* prop, const bool sync);

	void sendToSelf(sl::api::IKernel* pKernel, IObject* object);
	void sendToOthers(sl::api::IKernel* pKernel, IObject* object);

private:
	PropDelaySender*		_self;
	sl::api::IKernel*		_kernel;
	IHarbor*				_harbor;
	IEventEngine*			_eventEngine;
	IPacketSender*			_packetSender;
};
#endif