#include "Entity.h"
#include "IPythonEngine.h"
#include "slstring_utils.h"
#include "IDCCenter.h"
#include "GameDefine.h"
#include "NodeDefine.h"
#include "EntityMailBox.h"
#include "IScene.h"

SCRIPT_METHOD_DECLARE_BEGIN(Entity)
SCRIPT_METHOD_DECLARE("addProximity",  addProximity,        METH_VARARGS,   0)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_MEMBER_DECLARE_BEGIN(Entity)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(Entity)
SCRIPT_GET_DECLARE("base",              pyGetBaseMailBox,       0,              0)
SCRIPT_GET_DECLARE("spaceID",           pyGetSpaceID,           0,              0)
SCRIPT_GET_DECLARE("isWitnessed",       pyIsWitnessed,          0,              0)
SCRIPT_GETSET_DECLARE("position",       pyGetPosition,          pySetPosition,          0,			0)
SCRIPT_GETSET_DECLARE("direction",       pyGetDirection,          pySetDirection,          0,			0)
SCRIPT_GETSET_DECLARE_END()

BASE_SCRIPT_INIT(Entity, 0, 0, 0, 0, 0)
Entity::Entity(IObject* object, ScriptDefModule* pScriptModule, PyTypeObject* pyType, bool isInitialised)
	:_id(object->getID()),
    _baseMailBox(nullptr),
	EntityScriptObject(pyType, object, pScriptModule, isInitialised)
{
	float x = 0, y = 0, z = 0;
	SLMODULE(Scene)->getPosition(getInnerObject(), x, y, z);
	_pyPosition = NEW sl::pyscript::ScriptVector3(x, y, z); 
	
	float roll = 0, pitch = 0, yaw = 0;
	SLMODULE(Scene)->getDirection(getInnerObject(), roll, pitch, yaw);
	_pyDirection = NEW sl::pyscript::ScriptVector3(roll, pitch, yaw); 
}

Entity::~Entity(){
	SLASSERT(false, "wtf");
}

PyObject* Entity::addProximity(float range_xz, float range_y, int32 userarg){
	return PyLong_FromLong(13);
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

PyObject* Entity::pyIsWitnessed(){
	return PyBool_FromLong(false);
}

PyObject* Entity::pyGetPosition(){
	float x = 0, y = 0, z = 0;
	SLMODULE(Scene)->getPosition(getInnerObject(), x, y, z);
	_pyPosition->updateXYZ(x, y, z);
	Py_INCREF(_pyPosition);
	return _pyPosition;
}

int32 Entity::pySetPosition(PyObject* value){
	if(!sl::pyscript::ScriptVector3::check(value))
		return -1;

	Position3D pos;
	sl::pyscript::ScriptVector3::convertPyObjectToVector3(pos, value);
	_pyPosition->updateXYZ(pos.x, pos.y, pos.z);
	SLMODULE(Scene)->updatePosition(getInnerObject(), pos.x, pos.y, pos.z);
	return 0;
}

PyObject* Entity::pyGetDirection(){
	float roll = 0, pitch = 0, yaw = 0;
	SLMODULE(Scene)->getDirection(getInnerObject(), roll, pitch, yaw);
	_pyDirection->updateXYZ(roll, pitch, yaw);
	Py_INCREF(_pyPosition);
	return _pyPosition;
}

int32 Entity::pySetDirection(PyObject* value){
	if(!PySequence_Check(value)){
		PyErr_Format(PyExc_TypeError, "args of direction is not sequence");
		PyErr_PrintEx(0);
		return -1;
	}

	PyObject* pyItem = PySequence_GetItem(value, 0);
	if(!PyFloat_Check(pyItem)){
		PyErr_Format(PyExc_TypeError, "args of dirctions is not float");
		PyErr_PrintEx(0);
		Py_DECREF(pyItem);
		return -1;
	}
	float roll = float(PyFloat_AsDouble(pyItem));
	Py_DECREF(pyItem);
	
	pyItem = PySequence_GetItem(value, 1);
	if(!PyFloat_Check(pyItem)){
		PyErr_Format(PyExc_TypeError, "args of dirctions is not float");
		PyErr_PrintEx(0);
		Py_DECREF(pyItem);
		return -1;
	}
	float pitch = float(PyFloat_AsDouble(pyItem));
	Py_DECREF(pyItem);
	
	pyItem = PySequence_GetItem(value, 2);
	if(!PyFloat_Check(pyItem)){
		PyErr_Format(PyExc_TypeError, "args of dirctions is not float");
		PyErr_PrintEx(0);
		Py_DECREF(pyItem);
		return -1;
	}
	float yaw = float(PyFloat_AsDouble(pyItem));
	Py_DECREF(pyItem);

	_pyDirection->updateXYZ(roll, pitch, yaw);
	SLMODULE(Scene)->updatePosition(getInnerObject(), roll, pitch, yaw);
	return 0;
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
        PyObject* propName = PyUnicode_FromString(prop->getNameString());

		IDataType* dataType = (IDataType*)(prop->getExtra(getInnerObject()));
		PyObject* pyValue = (PyObject*)(dataType->createFromStream(stream));

		onScriptSetAttribute(propName, pyValue);	
	}
	return true;
}
