#ifndef SL_LOGIC_ENTITY_REMOTE_METHOD_H
#define SL_LOGIC_ENTITY_REMOTE_METHOD_H
#include "slikernel.h"
#include "IDCCenter.h"
#include "EntityScriptObject.h"
#include <string>

class EntityMailBox;
class RemoteEntityMethod: public EntityScriptObject{
	INSTANCE_SCRIPT_HREADER(RemoteEntityMethod, ScriptObject)
public:
	RemoteEntityMethod(EntityMailBox* mailBox, const IProp* prop);
	~RemoteEntityMethod(){};
	
    static PyObject* tp_call(PyObject* self, PyObject* args, PyObject* kwds);

    inline EntityMailBox* getMailBox() const {return _mailBox;}
    inline const IProp* getMethodProp() const {return _prop;}
    const char* getMethodName() const {return _prop->getNameString();}
	static void addArgsToStream(const int32 entityType, const IProp* methodProp, sl::IBStream& stream, PyObject* args);

private:
    EntityMailBox*      _mailBox;
    const IProp*        _prop;
};
#endif
