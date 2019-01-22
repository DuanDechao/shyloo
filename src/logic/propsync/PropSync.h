#ifndef _SL_FRAMEWORK_PROPSYNC_H_
#define _SL_FRAMEWORK_PROPSYNC_H_
#include "IPropSync.h"
#include "slsingleton.h"

class IHarbor;
class IEventEngine;
class IProp;
class IObject;
class IPacketSender;
class PropSync : public IPropSync, public sl::SLHolder<PropSync>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	void onCellEntityCreated(sl::api::IKernel* pKernel, const void* context, const int32 size);

	bool syncAllProps(IObject* object);
	void syncChangedProps(sl::api::IKernel* pKernel, IObject* object, const char* name, const IProp* prop, const bool sync);
	void sendToSelf(sl::api::IKernel* pKernel, IObject* object, const IProp* prop);
	void sendToOthers(sl::api::IKernel* pKernel, IObject* object, const IProp* prop);

private:
	void addPropDataToStream(IObject* object, const IProp* prop, IBStream& stream);

private:
	PropSync*		_self;
	sl::api::IKernel*		_kernel;
};
#endif
