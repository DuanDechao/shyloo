#ifndef _SL_FRAMEWORK_OCCOMMAND_H_
#define _SL_FRAMEWORK_OCCOMMAND_H_
#include "IOCCommand.h"
#include "slsingleton.h"
#include "slcallback.h"

class IHarbor;
class IObjectMgr;
class OArgs;
class IObjectLocator;
class OCCommand : public IOCCommand, public sl::SLHolder<OCCommand>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void rgsCommand(int32 cmdId, const CMD_CB_TYPE& func, const char* debug);

	virtual bool command(int32 cmdId, int64 sender, int64 receiver, const OArgs& args);
	virtual bool command(int32 cmdId, IObject* sender, int64 receiver, const OArgs& args);

	virtual bool commandToPlayer(int32 cmdId, int64 receiver, const OArgs& args);
	virtual bool commandToPlayer(int32 cmdId, int32 logic, int64 receiver, const OArgs& args);

	void onCommand(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args);
	void onLogicForwardCommand(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args);

private:
	OCCommand*			    _self;
	sl::api::IKernel*		_kernel;
	IHarbor*				_harbor;
	IObjectMgr*				_objectMgr;
	IObjectLocator*         _objectLocator;

	sl::CallBackType<int32, CMD_CB_TYPE>::type _cmdCBs;
};
#endif