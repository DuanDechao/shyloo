#include "Proxy.h"
#include "IEntityDef.h"
#include "IPythonEngine.h"
#include "slstring_utils.h"
#include "IDCCenter.h"
#include "GameDefine.h"
#include "NodeDefine.h"
#include "EntityMailBox.h"
#include "IMmoServer.h"

SCRIPT_METHOD_DECLARE_BEGIN(Proxy)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_MEMBER_DECLARE_BEGIN(Proxy)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(Proxy)
SCRIPT_GETSET_DECLARE_END()

BASE_SCRIPT_INIT(Proxy, 0, 0, 0, 0, 0)
Proxy::Proxy(uint64 entityId, IScriptDefModule* pScriptModule)
	:Base(entityId, pScriptModule, getScriptType(), true),
     _entityEnabled(false)
{}

Proxy::~Proxy(){
	SLASSERT(false, "wtf");
}

void Proxy::installScript(const char* name){
	refreshObjectType(name);
	INSTALL_SCRIPT_MODULE_TYPE(SLMODULE(PythonEngine), "Proxy", &s_objectType);
}

void Proxy::onEntityEnabled(){
    _entityEnabled = true;
    SCRIPT_OBJECT_CALL_ARGS0(this, "onEntityEnabled");
}
