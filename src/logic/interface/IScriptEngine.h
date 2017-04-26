#ifndef _SL_INTERFACE_SCRIPT_ENGINE_H__
#define _SL_INTERFACE_SCRIPT_ENGINE_H__
#include "slimodule.h"
class IScriptEngine: public sl::api::IModule{
public:
	virtual ~IScriptEngine() {}
};
#endif