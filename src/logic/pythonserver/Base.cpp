#include "Base.h"
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
SCRIPT_GET_DECLARE("isDestroyed",              pyGetIsDestroyed,            0,              0)
SCRIPT_GET_DECLARE("cell",              pyGetCellMailBox,            0,              0)
SCRIPT_GET_DECLARE("client",              pyGetClientMailBox,            0,              0)
SCRIPT_GET_DECLARE("databaseID",              pyGetDBID,            0,              0)
SCRIPT_GETSET_DECLARE_END()

BASE_SCRIPT_INIT(Base, 0, 0, 0, 0, 0)
Base::Base(IObject* object, ScriptDefModule* pScriptModule, PyTypeObject* pyType, bool isInitialised)
	:_id(object->getID()),
    _cellMailBox(nullptr),
    _clientMailBox(nullptr),
    _cellDataDict(nullptr),
	EntityScriptObject(pyType, object, pScriptModule, isInitialised)
{
    createCellData();
}

Base::~Base(){
	SLASSERT(false, "wtf[%s]", getScriptDefModule()->getModuleName() );
}

PyObject* Base::pyGetCellMailBox(){
    if(!_cellMailBox){
        ECHO_ERROR("not has cell mailbox");
        S_Return;
    }
	Py_INCREF(_cellMailBox);
    return _cellMailBox;
}

PyObject* Base::pyGetClientMailBox(){
	if(!_clientMailBox){
		ECHO_ERROR("not has client mailbox");
		S_Return;
	}
	Py_INCREF(_clientMailBox);
	return (PyObject*)_clientMailBox;
}

PyObject* Base::pyGetDBID(){
	return PyLong_FromUnsignedLongLong(getInnerObject()->getID());
}

PyObject* Base::pyGetIsDestroyed(){
	return PyBool_FromLong(0);
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
    
    remoteCreateCellEntity(cellMailBox->getRemoteNodeId(), cellMailBox->getEntityId());
    S_Return;
}

PyObject* Base::createInNewSpace(PyObject* args){
    int32 cellappIndex = 0;
    if(PyLong_Check(args))
        cellappIndex = (int32)PyLong_AsUnsignedLong(args);
    
    remoteCreateCellEntity(cellappIndex, 0);
    S_Return;
}

void Base::remoteCreateCellEntity(int32 cellappIdx, const uint64 createToObjectId){
	sl::BStream<10240> cellDataStream;
	addCellDataToStream(cellDataStream);
    IObject* innerObject = getInnerObject();
	sl::OBStream outData = cellDataStream.out();
    SLMODULE(BaseApp)->remoteCreateCellEntity(innerObject, createToObjectId, cellappIdx, _clientMailBox != NULL, outData.getContext(), outData.getSize());
}

void Base::addCellDataToStream(sl::IBStream& cellDataStream){
	PyObject* allProps = _pScriptModule->getProps();
    Py_ssize_t pos = 0;
    PyObject *key, *value;
	int32* propSize = cellDataStream.reserveInt32();
	*propSize = 0;
    while(PyDict_Next(allProps, &pos, &key, &value)){
	    if (!value){
            PyErr_Format(PyExc_AssertionError, "SetDefaultCellData:getCellPropData Prop is null!\n");
            PyErr_PrintEx(0);
		    return;
	    }
		
        wchar_t*wideCharString = PyUnicode_AsWideCharString(key, NULL);
		char* valStr = sl::CStringUtils::wchar2char(wideCharString);
		PyMem_Free(wideCharString);

		IObject* innerObject = getInnerObject();
        const IProp* prop = (const IProp*)PyLong_AsVoidPtr(value);
		if(!prop){
			SLASSERT(false, "cellprop[%s] is null", valStr);
			return;
		}

		if(prop->getSetting(innerObject) & prop_def::ObjectDataFlagRelation::OBJECT_CELL_DATA_FLAGS){
			IDataType* dataType = (IDataType*)(prop->getExtra(innerObject));
			cellDataStream << prop->getIndex(innerObject);
			PyObject* pyValue = PyDict_GetItem(_cellDataDict, key);
			dataType->addToStream(cellDataStream, pyValue);
			*propSize += 1; 
		}
    }
}

void Base::onGetCell(const int32 cellId){
    if(_cellMailBox){
        ECHO_ERROR("try set cell mailbox, but has one");
        return;
    }

    _cellMailBox = NEW EntityMailBox(EntityMailBoxType::MAILBOX_TYPE_CELL, cellId, getInnerObject()->getID(), _pScriptModule);

    Py_INCREF(_cellMailBox);

    SCRIPT_OBJECT_CALL_ARGS0(this, const_cast<char*>("onGetCell"));   
	if(PyErr_Occurred()){
		SLASSERT(false, "wtf");
	}
}

void Base::createCellData(){
    if(!_pScriptModule->hasCell() || !installCellDataAttr())
        return;
   
    _pScriptModule->setDefaultCellData(_cellDataDict); 
}

bool Base::installCellDataAttr(PyObject* dictData, bool installpy){
    if(dictData != NULL){
        if(_cellDataDict != dictData){
            if(_cellDataDict != NULL)
                Py_DECREF(_cellDataDict);

            _cellDataDict = dictData;
            Py_INCREF(_cellDataDict);
        }
    }

    if(installpy){
        if(_cellDataDict == NULL){
            _cellDataDict = PyDict_New();
        }

		PyObject* pyCellDataAttr = PyUnicode_FromString("cellData");
        //if(PyObject_SetAttrString(this, , _cellDataDict) == -1){
        if(ScriptObject::onScriptSetAttribute(pyCellDataAttr, _cellDataDict) == -1){
            SCRIPT_ERROR_CHECK();
            return false;
        }
    }
    return true;
}
