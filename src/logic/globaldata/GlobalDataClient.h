#ifndef _SL_FRAMEWORK_GLOBALDATA_CLIENT_H_
#define _SL_FRAMEWORK_GLOBALDATA_CLIENT_H_
#include "IGlobalData.h"
#include "GlobalData.h"
class GlobalDataClient : public IGlobalDataClient{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void addGlobalDataListener(IGlobalDataListener* listener);
	virtual bool write(const char* key, const int16 dataType, const void* data, const int32 dataSize);
	virtual bool del(const char* key);
	virtual const void* getData(const char* key, int16& dataType, int32& dataSize);
	virtual bool hasData(const char* key);
	virtual std::set<std::string>& getKeys() {return _globalData->getKeys(); }
	
	void sendGlobalDataChanged(const char* key, const int16 dataType, const void* data, const int32 dataSize, bool isDelete);
	void onGlobalDataChangedFromServer(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const sl::OBStream& args);

private:
	GlobalDataClient*			_self;
	sl::api::IKernel*			_kernel;
	GlobalData*					_globalData;
};
#endif
