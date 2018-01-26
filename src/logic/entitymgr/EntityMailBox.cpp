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
EntityMailBox::EntityMailBox(const int8 mailBoxType, const int32 nodeType, const int32 nodeId, IObject* object, IScriptDefModule* pScriptModule)
    :ScriptObject(getScriptType(), nullptr, 0, pScriptModule, false),
     _type(mailBoxType),
     _nodeType(nodeType),
     _nodeId(nodeId)
{
    setInnerObject(object);
}

PyObject * EntityMailBox::onScriptGetAttribute(PyObject* attr){
    wchar_t* pyUnicodeWideString = PyUnicode_AsWideCharString(attr, NULL);
    char* methodName = sl::CStringUtils::wchar2char(pyUnicodeWideString);
    PyMem_Free(pyUnicodeWideString);
    
    const IProp* prop = _pScriptModule->getMethodProp(getType(), attr);
    if(!prop){
       PyErr_Format(PyExc_AssertionError, "onScriptGetAttribute: can't get method %s", methodName);
       PyErr_PrintEx(0);
       return nullptr;
    }

    PyObject* remoteMethod = NEW RemoteEntityMethod(this, prop, methodName);
    Py_INCREF(remoteMethod);

    free(methodName);
    
    return remoteMethod;
}

bool EntityMailBox::postMail(const OArgs& data){
    SLMODULE(Harbor)->send(_nodeType, _nodeId, NodeProtocol::REMOTE_NEW_ENTITY_MAIL, data);   
    return true;
}

