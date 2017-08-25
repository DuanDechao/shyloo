#ifndef _SL_INTERFACE_STARTER_H__
#define _SL_INTERFACE_STARTER_H__
#include "slimodule.h"

class IStartStrategy{
public:
	virtual ~IStartStrategy() {}

	virtual void addSlave(const int32 nodeId) = 0;
	virtual int32 chooseNode(const int32 nodeType, const int32 nodeId) = 0;
};

class IStarter : public sl::api::IModule{
public:
	virtual ~IStarter() {}

	virtual void setStrategy(IStartStrategy* strategy) = 0;
};

#endif