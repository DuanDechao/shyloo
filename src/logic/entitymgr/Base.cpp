#include "Base.h"
#include "IEntityDef.h"
#include "IPythonEngine.h"
#include "slstring_utils.h"
#include "IDCCenter.h"
#include "GameDefine.h"
#include "NodeDefine.h"
#include "EntityMailBox.h"
#include "IMmoServer.h"

SCRIPT_METHOD_DECLARE_BEGIN(Base)
SCRIPT_METHOD_DECLARE("createCellEntity",  createCellEntity,        METH_VARARGS,   0)
SCRIPT_METHOD_DECLARE("createInNewSpace",  createInNewSpace,        METH_VARARGS,   0)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_MEMBER_DECLARE_BEGIN(Base)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(Base)
SCRIPT_GET_DECLARE("cell",              pyGetCellMailBox,            0,              0)
SCRIPT_GETSET_DECLARE_END()

BASE_SCRIPT_INIT(Base, 0, 0, 0, 0, 0)
Base::Base(uint64 entityId, IScriptDefModule* pScriptModule, PyTypeObject* pyType, bool isInitialised)
	:_id(entityId),
	_pScriptModule(pScriptModule),
    _cellMailBox(nullptr),
    _clientMailBox(nullptr),
	ScriptObject(pyType, pScriptModule->getModuleName(), entityId, isInitialised)
{}

Base::~Base(){
	SLASSERT(false, "wtf");
}

void Base::installScript(const char* name){
	refreshObjectType(name);
	INSTALL_SCRIPT_MODULE_TYPE(SLMODULE(PythonEngine), "Base", &s_objectType);
}

PyObject* Base::onScriptGetAttribute(PyObject* attr){
	PyObject* pyValue = _pScriptModule->scriptGetObjectAttribute(getInnerObject(), attr);
	if (!pyValue)
		return ScriptObject::onScriptGetAttribute(attr);
	return pyValue;
}

int Base::onScriptSetAttribute(PyObject* attr, PyObject* value){
	int32 ret = 0;
	if (!_pScriptModule || (ret = _pScriptModule->scriptSetObjectAttribute(getInnerObject(), attr, value)) < 0)
		return ScriptObject::onScriptSetAttribute(attr, value);
	return ret;
}

PyObject* Base::pyGetCellMailBox(){
    if(!_cellMailBox){
        ECHO_ERROR("not has cell mailbox");
        S_Return;
    }
    return (PyObject*)_cellMailBox;
}

PyObject* Base::createCellEntity(PyObject* pyObj){
    if(!PyObject_TypeCheck(pyObj, EntityMailBox::getScriptType())){
        PyErr_Format(PyExc_TypeError, "create %s arg1 is not cellMailBox!", _pScriptModule->getModuleName());
        PyErr_PrintEx(0);
        return 0;
    }

    EntityMailBox* cellMailBox = static_cast<EntityMailBox*>(pyObj);
    if(cellMailBox->getType() != EntityMailBoxType::MAILBOX_TYPE_CELL){
        PyErr_Format(PyExc_TypeError, "create %s args1 not is a direct cellMailBox!", _pScriptModule->getModuleName());
        PyErr_PrintEx(0);
        return 0;
    }
    
    IObject* innerObject = getInnerObject();
    SLMODULE(BaseApp)->remoteCreateCellEntity(innerObject, cellMailBox->getRemoteNodeId());
    printf("prepare creating cell entity...................................\n");
    S_Return;
}

PyObject* Base::createInNewSpace(PyObject* args){
    int32 cellappIndex = 0;
    if(PyLong_Check(args))
        cellappIndex = (int32)PyLong_AsUnsignedLong(args);
    
    IObject* innerObject = getInnerObject();
    SLMODULE(BaseApp)->remoteCreateCellEntity(innerObject, cellappIndex);
    S_Return;
}

void Base::onGetCell(const int32 cellId){
    if(_cellMailBox){
        ECHO_ERROR("try set cell mailbox, but has one");
        return;
    }

    IObject* innerObject = getInnerObject();
    _cellMailBox = NEW EntityMailBox(EntityMailBoxType::MAILBOX_TYPE_CELL, NodeType::SCENE, cellId, innerObject->getID(), innerObject->getObjectType());

    Py_INCREF(static_cast<PyObject*>(_cellMailBox));

    SCRIPT_OBJECT_CALL_ARGS0(this, const_cast<char*>("onGetCell"));   
}
