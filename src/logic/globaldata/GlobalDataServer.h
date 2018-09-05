#ifndef _SL_FRAMEWORK_GLOBALDATA_SERVER_H_
#define _SL_FRAMEWORK_GLOBALDATA_SERVER_H_
#include "IGlobalData.h"
#include "GlobalData.h"
class GlobalDataServer : public IGlobalDataServer , public IGlobalDataListener{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);	

	void addGlobalDataListener(IGlobalDataListener* listener);
	void onGlobalDataChanged(const char* key, const int16 dataType, const void* data, const int32 dataSize, bool isDelete);
	void onGlobalDataChangedFromClient(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const sl::OBStream& args);

private:
	GlobalDataServer*			_self;
	sl::api::IKernel*			_kernel;
	GlobalData*					_globalData;
	int32						_changedFromNodeType;
	int32						_changedFromNodeId;

};
#endif
