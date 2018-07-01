#include "Entity.h"
#include "IPythonEngine.h"
#include "slstring_utils.h"
#include "IDCCenter.h"
#include "GameDefine.h"
#include "NodeDefine.h"
#include "EntityMailBox.h"
#include "IScene.h"

SCRIPT_METHOD_DECLARE_BEGIN(Entity)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_MEMBER_DECLARE_BEGIN(Entity)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(Entity)
SCRIPT_GET_DECLARE("id",              pyGetID,            0,              0)
SCRIPT_GET_DECLARE("base",              pyGetBaseMailBox,            0,              0)
SCRIPT_GET_DECLARE("spaceID",              pyGetSpaceID,            0,              0)
SCRIPT_GETSET_DECLARE_END()

BASE_SCRIPT_INIT(Entity, 0, 0, 0, 0, 0)
Entity::Entity(IObject* object, ScriptDefModule* pScriptModule, PyTypeObject* pyType, bool isInitialised)
	:_id(object->getID()),
    _baseMailBox(nullptr),
	EntityScriptObject(pyType, object, pScriptModule, isInitialised)
{}

Entity::~Entity(){
	SLASSERT(false, "wtf");
}


PyObject* Entity::pyGetBaseMailBox(){
    if(!_baseMailBox){
        ECHO_ERROR("has no base mailbox");
        S_Return;
    }
    return (PyObject*)_baseMailBox;
}

PyObject* Entity::pyGetSpaceID(){
	IObject* innerObject = getInnerObject();
	int32 spaceId = SLMODULE(Scene)->getSpaceId(innerObject);
	return PyLong_FromLong(spaceId);
}

PyObject* Entity::pyGetID(){
	return PyLong_FromUnsignedLongLong(getInnerObject()->getID());
}

void Entity::setBaseMailBox(const int32 logic){
    if(_baseMailBox){
        ECHO_ERROR("should not have base mailbox");
        return;
    }
    const int32 entityType = SLMODULE(ObjectMgr)->getObjectType(_pScriptModule->getModuleName());
    _baseMailBox = NEW EntityMailBox(EntityMailBoxType::MAILBOX_TYPE_BASE, logic, getInnerObject()->getID(), _pScriptModule);

    Py_INCREF(static_cast<PyObject*>(_baseMailBox));
}

bool Entity::createCellDataFromStream(const void* cellData, const int32 cellDataSize){
	sl::OBStream stream((const char*)cellData, cellDataSize);
	int32 propSize = 0;
	if(!stream.readInt32(propSize))
		return false;

	for(int32 i = 0; i< propSize; i++){
		int32 uid = 0;
		if(!stream.readInt32(uid))
			return false;
	
		const IProp* prop = _pScriptModule->getPropByUid(uid);
        printf("current type name:%s\n", prop->getNameString());
        PyObject* propName = PyUnicode_FromString(prop->getNameString());

		IDataType* dataType = (IDataType*)(prop->getExtra(getInnerObject()));
		PyObject* pyValue = (PyObject*)(dataType->createFromStream(stream));

		onScriptSetAttribute(propName, pyValue);	
	}
	return true;
}
