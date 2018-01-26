#include "Entity.h"
#include "IEntityDef.h"
#include "IPythonEngine.h"
#include "slstring_utils.h"
#include "IDCCenter.h"
#include "GameDefine.h"
#include "NodeDefine.h"
#include "EntityMailBox.h"

SCRIPT_METHOD_DECLARE_BEGIN(Entity)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_MEMBER_DECLARE_BEGIN(Entity)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(Entity)
SCRIPT_GET_DECLARE("base",              pyGetBaseMailBox,            0,              0)
SCRIPT_GETSET_DECLARE_END()

BASE_SCRIPT_INIT(Entity, 0, 0, 0, 0, 0)
Entity::Entity(uint64 entityId, IScriptDefModule* pScriptModule, PyTypeObject* pyType, bool isInitialised)
	:_id(entityId),
    _baseMailBox(nullptr),
	ScriptObject(pyType, pScriptModule->getModuleName(), entityId, pScriptModule, isInitialised)
{}

Entity::~Entity(){
	SLASSERT(false, "wtf");
}

void Entity::installScript(const char* name){
	refreshObjectType(name);
	INSTALL_SCRIPT_MODULE_TYPE(SLMODULE(PythonEngine), "Entity", &s_objectType);
}

PyObject* Entity::onScriptGetAttribute(PyObject* attr){
	PyObject* pyValue = _pScriptModule->scriptGetObjectAttribute(this, attr);
	if (!pyValue)
		return ScriptObject::onScriptGetAttribute(attr);
	return pyValue;
}

int Entity::onScriptSetAttribute(PyObject* attr, PyObject* value){
	int32 ret = 0;
	if (!_pScriptModule || (ret = _pScriptModule->scriptSetObjectAttribute(this, attr, value)) < 0)
		return ScriptObject::onScriptSetAttribute(attr, value);
	return ret;
}

PyObject* Entity::pyGetBaseMailBox(){
    if(!_baseMailBox){
        ECHO_ERROR("has no base mailbox");
        S_Return;
    }
    return (PyObject*)_baseMailBox;
}

void Entity::onRemoteMethodCall(const OArgs& args, int32& idx){
    const char* methodName = args.getString(idx++);
    PyObject* pyFunc = PyObject_GetAttrString(this, const_cast<char*>(methodName));
    PyObject_CallObject(pyFunc, NULL);
}

void Entity::setBaseMailBox(const int32 logic){
    if(_baseMailBox){
        ECHO_ERROR("should not have base mailbox");
        return;
    }
    const int32 entityType = SLMODULE(ObjectMgr)->getObjectType(_pScriptModule->getModuleName());
    _baseMailBox = NEW EntityMailBox(EntityMailBoxType::MAILBOX_TYPE_BASE, NodeType::LOGIC, logic, getInnerObject(), _pScriptModule);

    Py_INCREF(static_cast<PyObject*>(_baseMailBox));
}

