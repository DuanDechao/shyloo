#ifndef _SL_INTERFACE_SCENE_H__
#define _SL_INTERFACE_SCENE_H__
#include "slimodule.h"
#include <functional>

class IScene : public sl::api::IModule{
public:
	virtual ~IScene() {}
};

#endif