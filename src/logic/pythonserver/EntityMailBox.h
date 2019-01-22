#ifndef SL_LOGIC_ENTITY_MAILBOX_H
#define SL_LOGIC_ENTITY_MAILBOX_H
#include "slikernel.h"
#include "IDCCenter.h"
#include "pyscript/scriptobject.h"
#include "ScriptDefModule.h"
using namespace sl::pyscript;
class EntityMailBox: public sl::pyscript::ScriptObject{
	INSTANCE_SCRIPT_HREADER(EntityMailBox, ScriptObject)

public:
	EntityMailBox(const int8 mailBoxType, const int32 nodeId, const uint64 entityId, ScriptDefModule* pScriptModule);
	~EntityMailBox(){}
	
	PyObject * onScriptGetAttribute(PyObject* attr);
	static void installScript(const char* name){}

    inline int8 getType() const {return _type;}
    inline int32 getRemoteNodeId() const {return _nodeId;}
	inline uint64 getEntityId() const {return _entityId;}
	inline int32 getEntityType() const {return _pScriptModule->getModuleType(); }
	inline ScriptDefModule* getScriptDefModule() const {return _pScriptModule;}
    
	bool newMail(sl::IBStream& stream);
	bool postMail(const sl::OBStream& data);

	static PyObject* __unpickle__(PyObject* self, PyObject* args);
	static void onInstallScript(PyObject* mod);

	//支持pickler方法
	static PyObject* __py_reduce_ex__(PyObject* self, PyObject* protocol);
	static PyObject* tryGetEntity(const int16 mailBoxType, const int32 nodeId, const uint64 entityId);

private:
    int8						_type;
    int32						_nodeId;
	uint64						_entityId;
    ScriptDefModule*			_pScriptModule;
};
#endif
