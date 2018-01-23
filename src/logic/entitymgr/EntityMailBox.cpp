#include "EntityMailBox.h"
#include "NodeDefine.h"
#include "IHarbor.h"
#include "NodeProtocol.h"
#include "IEntityDef.h"
#include "RemoteEntityMethod.h"

SCRIPT_METHOD_DECLARE_BEGIN(EntityMailBox)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_MEMBER_DECLARE_BEGIN(EntityMailBox)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(EntityMailBox)
SCRIPT_GETSET_DECLARE_END()

BASE_SCRIPT_INIT(EntityMailBox, 0, 0, 0, 0, 0)
EntityMailBox::EntityMailBox(const int8 mailBoxType, const int32 nodeType, const int32 nodeId, const uint64 entityId, const int32 entityType)
    :ScriptObject(getScriptType(), nullptr, 0, false),
     _type(mailBoxType),
     _nodeType(nodeType),
     _nodeId(nodeId),
     _entityId(entityId),
     _entityType(entityType),
     _scriptDefModule(nullptr),
     _innerObject(nullptr)
{
    _scriptDefModule = SLMODULE(EntityDef)->findScriptDefModule(_entityType);
    _innerObject = SLMODULE(ObjectMgr)->findObject(_entityId);
}

PyObject * EntityMailBox::onScriptGetAttribute(PyObject* attr){
    const IProp* prop = _scriptDefModule->getMethodProp(getType(), attr);
    if(!prop)
       return nullptr;

    wchar_t* pyUnicodeWideString = PyUnicode_AsWideCharString(attr, NULL);
    char* methodName = sl::CStringUtils::wchar2char(pyUnicodeWideString);
    PyMem_Free(pyUnicodeWideString);

    PyObject* remoteMethod = NEW RemoteEntityMethod(this, prop, methodName);
    Py_INCREF(remoteMethod);
    
    return remoteMethod;
}

bool EntityMailBox::postMail(const OArgs& data){
    SLMODULE(Harbor)->send(NodeType::SCENE, 1, NodeProtocol::REMOTE_NEW_ENTITY_MAIL, data);   
    return true;
}

