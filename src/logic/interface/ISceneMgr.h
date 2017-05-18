#ifndef _SL_INTERFACE_SCENEMGR_H__
#define _SL_INTERFACE_SCENEMGR_H__
#include "slimodule.h"
#include <functional>

class ISceneMgr : public sl::api::IModule{
public:
	virtual ~ISceneMgr() {}
};

#endif