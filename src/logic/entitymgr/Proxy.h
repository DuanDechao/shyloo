#ifndef SL_LOGIC_PROXY_H
#define SL_LOGIC_PROXY_H
#include "slikernel.h"
#include "ScriptObject.h"
#include "IEntityMgr.h"
#include "Base.h"
class IScriptDefModule;
class EntityMailBox;

class Proxy: public Base{
	BASE_SCRIPT_HEADER(Proxy, Base)
public:
	Proxy(uint64 entityId, IScriptDefModule* pScriptModule);
	~Proxy();

    static void installScript(const char* name);

    inline void setAgentId(int64 agentId) {_agentId = agentId;}
    inline int64 getAgentId() {return _agentId;}

    void onEntityEnabled();

private:
    int64           _agentId;
    bool            _entityEnabled;
};
#endif
