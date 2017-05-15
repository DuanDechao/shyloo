#ifndef _SL_FRAMEWORK_PACKETSENDER_H_
#define _SL_FRAMEWORK_PACKETSENDER_H_
#include "IPacketSender.h"
#include "slsingleton.h"
class IHarbor;
class PacketSender : public IPacketSender, public sl::SLHolder<PacketSender>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void send(int32 gate, int64 id, int32 messageId, const void* context, const int32 size, int8 delay);

private:
	PacketSender*			_self;
	sl::api::IKernel*		_kernel;
	IHarbor*				_harbor;
};
#endif