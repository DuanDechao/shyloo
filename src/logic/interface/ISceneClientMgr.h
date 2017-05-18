#ifndef _SL_INTERFACE_SCENECLIENTMGR_H__
#define _SL_INTERFACE_SCENECLIENTMGR_H__
#include "slimodule.h"
#include <functional>

class ISceneClientMgr : public sl::api::IModule{
public:
	virtual ~ISceneClientMgr() {}
};

#endif