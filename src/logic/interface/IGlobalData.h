#ifndef _SL_INTERFACE_GLOBAL_DATA_H__
#define _SL_INTERFACE_GLOBAL_DATA_H__
#include "slimodule.h"
#include <set>
class IGlobalDataListener{
public:
	virtual ~IGlobalDataListener(){}
	virtual void onGlobalDataChanged(const char* key, const int16 dataType, const void* data, const int32 dataSize, bool isDelete) = 0;
};

class IGlobalDataServer : public sl::api::IModule{
public:
	virtual ~IGlobalDataServer() {}
};

class IGlobalDataClient : public sl::api::IModule{
public:
	virtual ~IGlobalDataClient() {}
	virtual void addGlobalDataListener(IGlobalDataListener* listener) = 0;
	virtual bool write(const char* key, const int16 dataType, const void* data, const int32 dataSize) = 0;
	virtual bool del(const char* key) = 0;
	virtual const void* getData(const char* key, int16& dataType, int32& dataSize) = 0;
	virtual bool hasData(const char* key) = 0;
	virtual std::set<std::string>& getKeys() = 0;
};

#endif
