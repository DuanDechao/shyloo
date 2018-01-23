#include "RemoteEntityMethod.h"
#include "EntityMailBox.h"
#include "NodeProtocol.h"

SCRIPT_METHOD_DECLARE_BEGIN(RemoteEntityMethod)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_MEMBER_DECLARE_BEGIN(RemoteEntityMethod)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(RemoteEntityMethod)
SCRIPT_GETSET_DECLARE_END()

SCRIPT_INIT(RemoteEntityMethod, tp_call, 0, 0, 0, 0)
RemoteEntityMethod::RemoteEntityMethod(EntityMailBox* mailBox, const IProp* prop, const char* methodName)
    :ScriptObject(getScriptType(), nullptr, 0, false),
     _mailBox(mailBox),
     _prop(prop),
     _methodName(methodName)
{}

PyObject * RemoteEntityMethod::tp_call(PyObject* self, PyObject* args, PyObject* kwds){
    RemoteEntityMethod* rMethod = static_cast<RemoteEntityMethod*>(self);
    EntityMailBox* mailbox = rMethod->getMailBox();
    IScriptDefModule* scriptDefModule = mailbox->getScriptDefModule();
    IObject* object = mailbox->getMMObject();
    const IProp* methodProp = rMethod->getMethodProp();
  //  if(scriptDefModule->checkMethodArgs(mailbox->getMMObject(), methodProp, args)){
        IArgs<10, 1000> inArgs;
        inArgs << object->getID();
        inArgs << rMethod->getMethodName();
        inArgs << 2;
        inArgs.fix();
        printf("remote send mail+_++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        mailbox->postMail(inArgs.out());
    //}
    
    S_Return;
}

