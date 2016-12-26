#ifndef _IHARBOR_H_
#define _IHARBOR_H_
#include "slikernel.h"
#include "slimodule.h"
typedef void(*node_cb)(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* pContext, const int32 size);
class IHarbor : public sl::api::IModule
{
public:
	virtual ~IHarbor() {}
};
#endif