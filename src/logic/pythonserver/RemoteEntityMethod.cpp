#include "RemoteEntityMethod.h"
#include "EntityMailBox.h"
#include "NodeProtocol.h"
#include "slbinary_stream.h"
SCRIPT_METHOD_DECLARE_BEGIN(RemoteEntityMethod)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_MEMBER_DECLARE_BEGIN(RemoteEntityMethod)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(RemoteEntityMethod)
SCRIPT_GETSET_DECLARE_END()

SCRIPT_INIT(RemoteEntityMethod, tp_call, 0, 0, 0, 0)
RemoteEntityMethod::RemoteEntityMethod(EntityMailBox* mailBox, const IProp* prop)
    :ScriptObject(getScriptType(), false),
     _mailBox(mailBox),
     _prop(prop)
{}

PyObject * RemoteEntityMethod::tp_call(PyObject* self, PyObject* args, PyObject* kwds){
    RemoteEntityMethod* rMethod = static_cast<RemoteEntityMethod*>(self);
    EntityMailBox* mailbox = rMethod->getMailBox();
    ScriptDefModule* scriptDefModule = mailbox->getScriptDefModule();
    const IProp* methodProp = rMethod->getMethodProp();
	sl::BStream<1000> stream;
	mailbox->newMail(stream);
  //  if(scriptDefModule->checkMethodArgs(mailbox->getMMObject(), methodProp, args)){
	stream << (int16)methodProp->getIndex(mailbox->getEntityType());
	addArgsToStream(mailbox->getEntityType(), methodProp, stream, args);
    printf("remote send mail+_++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    mailbox->postMail(stream.out());
    //}
    S_Return;
}

void RemoteEntityMethod::addArgsToStream(const int32 entityType, const IProp* methodProp, sl::IBStream& stream, PyObject* args){
	int32 argsCount = methodProp->getSize(entityType);
	for(int32 i = 0; i < argsCount; i++){
		PyObject* pyArg = PyTuple_GetItem(args, i);
		IDataType** argType = (IDataType**)(methodProp->getExtra(entityType));
		argType[i]->addToStream(stream, pyArg);
	}
}

