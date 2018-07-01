#include "Proxy.h"
#include "IPythonEngine.h"
#include "slstring_utils.h"
#include "IDCCenter.h"
#include "GameDefine.h"
#include "NodeDefine.h"
#include "EntityMailBox.h"
#include "IMmoServer.h"

SCRIPT_METHOD_DECLARE_BEGIN(Proxy)
SCRIPT_METHOD_DECLARE("giveClientTo",  pyGiveClientTo,        METH_VARARGS,   0)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_MEMBER_DECLARE_BEGIN(Proxy)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(Proxy)
SCRIPT_GETSET_DECLARE_END()

BASE_SCRIPT_INIT(Proxy, 0, 0, 0, 0, 0)
Proxy::Proxy(IObject* object, ScriptDefModule* pScriptModule)
	:Base(object, pScriptModule, getScriptType(), true),
     _entityEnabled(false)
{}

Proxy::~Proxy(){
	SLASSERT(false, "wtf");
}

void Proxy::onEntityEnabled(const int64 agentId){
	if(agentId <= 0)
		return;

    if(_clientMailBox){
        ECHO_ERROR("try set cell mailbox, but has one");
        return;
    }
    _clientMailBox = NEW EntityMailBox(EntityMailBoxType::MAILBOX_TYPE_CLIENT, agentId, getInnerObject()->getID(), _pScriptModule);
    Py_INCREF(static_cast<PyObject*>(_clientMailBox));
    
	_entityEnabled = true;
    SCRIPT_OBJECT_CALL_ARGS0(this, "onEntityEnabled");
}

PyObject* Proxy::pyGiveClientTo(PyObject* pyOtherProxy){
	Proxy* otherProxy = static_cast<Proxy*>(pyOtherProxy);
	giveClientTo(otherProxy);
	S_Return;
}

void Proxy::giveClientTo(Proxy* proxy){
	if(_clientMailBox == NULL){
		ECHO_ERROR("Proxy[%s]::giveClientTo: no has client.", getScriptDefModule()->getModuleName());
		return;
	}

	if(getCellMailBox()){
		//通知cell丢失witness
	}
	
	const int32 remoteNodeId = getClientMailBox()->getRemoteNodeId(); 
	setClientMailBox(NULL);
	proxy->onGiveClientTo(remoteNodeId);

}

void Proxy::onGiveClientTo(const int32 nodeId){
	setClientMailBox(NEW EntityMailBox(EntityMailBoxType::MAILBOX_TYPE_CLIENT, nodeId, getInnerObject()->getID(), getScriptDefModule()));
	SLMODULE(BaseApp)->proxyCreated(getInnerObject(), nodeId);
}
