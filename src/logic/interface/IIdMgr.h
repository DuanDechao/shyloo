#ifndef __SL_INTERFACE_IIDMGR_H__
#define __SL_INTERFACE_IIDMGR_H__
#include "slimodule.h"

class IIdMgr : public sl::api::IModule{
public:
	virtual ~IIdMgr() {}

	virtual uint64 allocID() = 0;
};
#endif