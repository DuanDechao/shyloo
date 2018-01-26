#ifndef SL_LOGIC_ENTITY_MAILBOX_H
#define SL_LOGIC_ENTITY_MAILBOX_H
#include "slikernel.h"
#include "ScriptObject.h"
#include "IEntityMgr.h"
#include "IDCCenter.h"
#include "ScriptObject.h"

class IScriptDefModule;
class EntityMailBox: public ScriptObject{
	BASE_SCRIPT_HEADER(EntityMailBox, ScriptObject)
public:
	EntityMailBox(const int8 mailBoxType, const int32 nodeType, const int32 nodeId, IObject* object, IScriptDefModule* pScriptModule);
	~EntityMailBox(){}
	
	PyObject * onScriptGetAttribute(PyObject* attr);
	static void installScript(const char* name){}

    inline int8 getType() const {return _type;}
    bool postMail(const OArgs& data);
    
    inline int32 getRemoteNodeId() {return _nodeId;}

private:
    int8             _type;
    int32            _nodeType;
    int32            _nodeId;
};
#endif
