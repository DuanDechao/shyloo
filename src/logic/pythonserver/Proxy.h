#ifndef SL_LOGIC_PROXY_H
#define SL_LOGIC_PROXY_H
#include "slikernel.h"
#include "EntityScriptObject.h"
#include "Base.h"
class IScriptDefModule;
class EntityMailBox;

class Proxy: public Base{
	BASE_SCRIPT_HREADER(Proxy, Base)
public:
	Proxy(IObject* object, ScriptDefModule* pScriptModule);
	~Proxy();
    
	DECLARE_PY_MOTHOD_ARG1(pyGiveClientTo, PyObject_ptr);
	
	void onEntityEnabled(const int64 agentId);
	void giveClientTo(Proxy* proxy);
	void onGiveClientTo(const int32 nodeId);

private:
    bool            _entityEnabled;
};
#endif
