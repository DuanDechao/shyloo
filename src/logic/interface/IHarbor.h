#ifndef _IHARBOR_H_
#define _IHARBOR_H_
#include "slikernel.h"
#include "slimodule.h"
#include "slargs.h"
class INodeListener
{
public:
	virtual ~INodeListener() {}
	virtual void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port) = 0;
	virtual void onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId) = 0;
};

typedef void(*node_cb)(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* pContext, const int32 size);
typedef void(*node_args_cb)(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
class IHarbor : public sl::api::IModule
{
public:
	virtual ~IHarbor() {}
	virtual void addNodeListener(INodeListener* pNodeListener) = 0;
};
#endif