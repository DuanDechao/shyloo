#include "EntityMailBox.h"
#include "NodeDefine.h"
#include "IHarbor.h"
#include "NodeProtocol.h"
#include "IObjectDef.h"
#include "RemoteEntityMethod.h"
#include "IGate.h"
#include "ProtocolID.pb.h"
#include "GameDefine.h"
#include "PythonServer.h"
#include "IPythonEngine.h"
SCRIPT_METHOD_DECLARE_BEGIN(EntityMailBox)
SCRIPT_METHOD_DECLARE("__reduce_ex__",  reduce_ex__,        METH_VARARGS,   0)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_MEMBER_DECLARE_BEGIN(EntityMailBox)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(EntityMailBox)
SCRIPT_GETSET_DECLARE_END()

BASE_SCRIPT_INIT(EntityMailBox, 0, 0, 0, 0, 0)
EntityMailBox::EntityMailBox(const int8 mailBoxType, const int32 nodeId, const uint64 entityId, ScriptDefModule* pScriptModule)
    :ScriptObject(getScriptType(), false),
	 _type(mailBoxType),
     _nodeId(nodeId),
	 _entityId(entityId),
	 _pScriptModule(pScriptModule)
{}

PyObject * EntityMailBox::onScriptGetAttribute(PyObject* attr){
    const IProp* prop = _pScriptModule->getMethodProp(getType(), attr);
    if(prop){
		PyObject* remoteMethod = NEW RemoteEntityMethod(this, prop);
	    Py_INCREF(remoteMethod);
		return remoteMethod;
    }

	wchar_t* wideCharString = PyUnicode_AsWideCharString(attr, NULL);
	char* strAttr = sl::CStringUtils::wchar2char(wideCharString);
	PyMem_Free(wideCharString);

	const char* currMailBoxName = ENTITY_MAILBOX_NAME_MAPPING[_type];
	if(strcmp(strAttr, currMailBoxName) == 0){
		printf("cant get mailBox!!!\n");
		SLASSERT(false, "wtf");
		return NULL;
	}

	int16 mType = -1;
	if(strcmp(strAttr, "cell") == 0){
		if(_type == EntityMailBoxType::MAILBOX_TYPE_BASE_VIA_CELL)
			mType = EntityMailBoxType::MAILBOX_TYPE_CELL;
		else
			mType = EntityMailBoxType::MAILBOX_TYPE_CELL_VIA_BASE;
	}
	else if(strcmp(strAttr, "base") == 0){
		if(_type == EntityMailBoxType::MAILBOX_TYPE_CELL_VIA_BASE)
			mType = EntityMailBoxType::MAILBOX_TYPE_BASE;
		else
			mType = EntityMailBoxType::MAILBOX_TYPE_BASE_VIA_CELL;
	}
	else if(strcmp(strAttr, "client") == 0){
		if(_type == EntityMailBoxType::MAILBOX_TYPE_BASE)
			mType = EntityMailBoxType::MAILBOX_TYPE_CLIENT_VIA_BASE;
		else if(_type == EntityMailBoxType::MAILBOX_TYPE_CELL)
			mType = EntityMailBoxType::MAILBOX_TYPE_CLIENT_VIA_CELL;
	}

	if(mType != -1){
		free(strAttr);
		return NEW EntityMailBox(mType, _nodeId, _entityId, _pScriptModule);
	}

	free(strAttr);

	PyObject* pyValue = ScriptObject::onScriptGetAttribute(attr);
	if(!pyValue){
       PyErr_Format(PyExc_AssertionError, "onScriptGetAttribute: can't get method %s", prop->getNameString());
       PyErr_PrintEx(0);
       return nullptr;
	}
	
	return pyValue;
}

bool EntityMailBox::newMail(sl::IBStream& stream){
	stream << _type;
	stream << _entityId;
}

bool EntityMailBox::postMail(const sl::OBStream& data){
	if(_type == EntityMailBoxType::MAILBOX_TYPE_CELL || _type == EntityMailBoxType::MAILBOX_TYPE_BASE_VIA_CELL 
		|| _type == EntityMailBoxType::MAILBOX_TYPE_CLIENT_VIA_CELL)	
		SLMODULE(Harbor)->send(NodeType::SCENE, _nodeId, NodeProtocol::REMOTE_NEW_ENTITY_MAIL, data);   
	if(_type == EntityMailBoxType::MAILBOX_TYPE_BASE || _type == EntityMailBoxType::MAILBOX_TYPE_CELL_VIA_BASE 
		|| _type == EntityMailBoxType::MAILBOX_TYPE_CLIENT_VIA_BASE)	
		SLMODULE(Harbor)->send(NodeType::LOGIC, _nodeId, NodeProtocol::REMOTE_NEW_ENTITY_MAIL, data);   
	if(_type == EntityMailBoxType::MAILBOX_TYPE_CLIENT)	
		SLMODULE(Gate)->sendToClient(_nodeId, ServerMsgID::SERVER_MSG_REMOTE_NEW_ENTITY_MAIL, data);
    return true;
}

PyObject* EntityMailBox::__unpickle__(PyObject* self, PyObject* args){
	uint64 objectId = 0;
	int32 nodeId = 0;
	int32 objectType = 0;
	int16 mailboxType = 0;

	Py_ssize_t size = PyTuple_Size(args);
	if(size != 4){
		ECHO_ERROR("EntityMailBox::__unpickle__: args is error! size != 4");
		S_Return;
	}

	if(!PyArg_ParseTuple(args, "Kiih", &objectId, &nodeId, &objectType, &mailboxType)){
		ECHO_ERROR("EntityMailBox::__unpickle__: args is error!");
		S_Return;
	}

	ScriptDefModule* defModule = PythonServer::getInstance()->findScriptDefModule(objectType);
	if( NULL == defModule ){
		ECHO_ERROR("EntityMailBox::__unpickle__: not found object Type[%d]", objectType);
		S_Return;
	}

	PyObject* entity = tryGetEntity(mailboxType, nodeId, objectId);
	if(entity){
		SLASSERT(false, "tsda");
		Py_INCREF(entity);
		return entity;
	}

	return NEW EntityMailBox(mailboxType, nodeId, objectId, defModule);
}

PyObject* EntityMailBox::tryGetEntity(const int16 mailboxType, const int32 nodeId, const uint64 entityId){
	const int32 nodeType = ENTITY_MAILBOX_NODE_TYPE_MAPPING[mailboxType];
	if(nodeType == SLMODULE(Harbor)->getNodeType() && nodeId == SLMODULE(Harbor)->getNodeId())
		return (PyObject*)(PythonServer::getInstance()->findEntity(entityId));
	
	return nullptr;
}


void EntityMailBox::onInstallScript(PyObject* mod){
	static PyMethodDef __unpickle_method = {"MailBox", (PyCFunction)&EntityMailBox::__unpickle__, METH_VARARGS, 0};
	PyObject* pyFunc = PyCFunction_New(&__unpickle_method, NULL);
	SLMODULE(PythonEngine)->registerUnpickleFunc(pyFunc, "MailBox");
	Py_DECREF(pyFunc);
}

PyObject* EntityMailBox::__py_reduce_ex__(PyObject* self, PyObject* protocol){
	EntityMailBox* mailbox = static_cast<EntityMailBox*>(self);
	PyObject* unpickleMethod = SLMODULE(PythonEngine)->getUnpickleFunc("MailBox");
	if(unpickleMethod == NULL){
		return NULL;
	}
	
	PyObject* args = PyTuple_New(2);
	PyTuple_SET_ITEM(args, 0, unpickleMethod);

	PyObject* args1 = PyTuple_New(4);
	PyTuple_SET_ITEM(args1, 0, PyLong_FromUnsignedLongLong(mailbox->getEntityId()));
	PyTuple_SET_ITEM(args1, 1, PyLong_FromLong(mailbox->getRemoteNodeId()));
	PyTuple_SET_ITEM(args1, 2, PyLong_FromLong(mailbox->getEntityType()));
	PyTuple_SET_ITEM(args1, 3, PyLong_FromLong(mailbox->getType()));

	PyTuple_SET_ITEM(args, 1, args1);
	return args;
}
