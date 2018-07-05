#include "EntityMailBox.h"
#include "NodeDefine.h"
#include "IHarbor.h"
#include "NodeProtocol.h"
#include "IObjectDef.h"
#include "RemoteEntityMethod.h"
#include "IGate.h"
#include "ProtocolID.pb.h"
#include "EntityMgr.h"
#include "IPythonEngine.h"
#include "GameDefine.h"
void MailBoxType::addToStream(sl::IBStream& stream, void* value){
	PyObject* pyValue = (PyObject*)value;
	const char types[2][10] = {
		"Entity",
		"Base"
	};

	uint64 entityId = 0;
	int32 nodeId = 0;
	int16 mType = 0;
	int32 entityType = 0;
	if(pyValue != Py_None){
		for(int32 i =0; i < 2; i++){
			PyTypeObject* stype = sl::pyscript::ScriptObject::getScriptObjectType(types[i]); 
			if(stype == NULL)
				continue;

			if(PyObject_IsInstance(pyValue, (PyObject*)stype)){
				PyObject* pyid = PyObject_GetAttrString(pyValue, "id");
				if(pyid){
					entityId = PyLong_AsUnsignedLongLong(pyid);
					Py_DECREF(pyid);
				}
				else{
					SCRIPT_ERROR_CHECK();
					entityId = 0;
					nodeId = 0;
					break;
				}

				nodeId = SLMODULE(Harbor)->getNodeId();
				int32 nodeType = SLMODULE(Harbor)->getNodeType();
				if(nodeType == NodeType::LOGIC)
					mType = MAILBOX_TYPE_BASE;
				else if(nodeType == NodeType::SCENE)
					mType = MAILBOX_TYPE_CELL;
				else
					mType = MAILBOX_TYPE_CLIENT;

				EntityScriptObject* pyScriptEntity = static_cast<EntityScriptObject*>(pyValue);
				entityType = pyScriptEntity->getInnerObject()->getObjectType();
			}
		}

		if(entityId == 0){
			EntityMailBox* pMailBox = static_cast<EntityMailBox*>(pyValue);
			nodeId = pMailBox->getRemoteNodeId();
			entityId = pMailBox->getEntityId();
			mType = pMailBox->getType();
			entityType = pMailBox->getEntityType();
		}
	}

	stream << entityId;
	stream << nodeId;
	stream << mType;
	stream << entityType;
}

void MailBoxType::addToObject(IObject* object, const IProp* prop, void* value){
	int32 size = 0;
	const char* data = (const char*)(object->getPropData(prop, size));
	sl::IBStream stream(const_cast<char*>(data), size);
	addToStream(stream, value);
}
	
void* MailBoxType::createFromStream(const sl::OBStream& stream){
	uint64 entityId = 0;
	int32 nodeId = 0;
	int16 mType = 0;
	int32 entityType = 0;
	if(!stream.readUint64(entityId) || !stream.readInt32(nodeId) || !stream.readInt16(mType) || !stream.readInt32(entityType)){
		ECHO_ERROR("MailBoxType::createFromStream: read data error");
		return nullptr;
	}

	if(entityId > 0){
		PyObject* entity = EntityMailBox::tryGetEntity(mType, nodeId, entityId);
		if(entity){
			Py_INCREF(entity);
			return entity;
		}

		return NEW EntityMailBox(mType, nodeId, entityId, EntityMgr::getInstance()->findScriptDefModule(entityType));
	}

	Py_RETURN_NONE;
}

void* MailBoxType::createFromObject(IObject* object, const IProp* prop){
	int32 size = 0;
	const void* data = object->getPropData(prop, size);
	const sl::OBStream stream((const char*)data, size);
	return createFromStream(stream);
}

const int32 MailBoxType::getSize() const{
	return sizeof(uint64) + 2 * sizeof(int32) + sizeof(int16);
}

void MailBoxType::addDataTypeInfo(sl::IBStream& stream){
	stream << getUid();                                                                                                                                                                                                                                     
	stream << getName();
	stream << getAliasName();
}

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
	PyObject* pyValue = ScriptObject::onScriptGetAttribute(attr);
	if(pyValue)
		return pyValue;
    
    const IProp* prop = _pScriptModule->getMethodProp(getType(), attr);
    if(!prop){
       PyErr_Format(PyExc_AssertionError, "onScriptGetAttribute: can't get method %s", prop->getNameString());
       PyErr_PrintEx(0);
       return nullptr;
    }

    PyObject* remoteMethod = NEW RemoteEntityMethod(this, prop);
    Py_INCREF(remoteMethod);
    
    return remoteMethod;
}

bool EntityMailBox::postMail(const sl::OBStream& data){
	//if(_type == EntityMailBoxType::MAILBOX_TYPE_CELL)	
		//SLMODULE(Harbor)->send(NodeType::SCENE, _nodeId, NodeProtocol::REMOTE_NEW_ENTITY_MAIL, data);   
	//if(_type == EntityMailBoxType::MAILBOX_TYPE_BASE)	
		//SLMODULE(Harbor)->send(NodeType::LOGIC, _nodeId, NodeProtocol::REMOTE_NEW_ENTITY_MAIL, data);   
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

	ScriptDefModule* defModule = EntityMgr::getInstance()->findScriptDefModule(objectType);
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
		return (PyObject*)(EntityMgr::getInstance()->findEntity(entityId));
	
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
