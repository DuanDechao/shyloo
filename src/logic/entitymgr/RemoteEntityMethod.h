#ifndef SL_LOGIC_ENTITY_REMOTE_METHOD_H
#define SL_LOGIC_ENTITY_REMOTE_METHOD_H
#include "slikernel.h"
#include "IEntityMgr.h"
#include "IDCCenter.h"
#include "ScriptObject.h"
#include <string>

class EntityMailBox;
class RemoteEntityMethod: public ScriptObject{
	BASE_SCRIPT_HEADER(RemoteEntityMethod, ScriptObject)
public:
	RemoteEntityMethod(EntityMailBox* mailBox, const IProp* prop, const char* methodName);
	~RemoteEntityMethod(){};
	
    static PyObject* tp_call(PyObject* self, PyObject* args, PyObject* kwds);

    inline EntityMailBox* getMailBox() const {return _mailBox;}
    inline const IProp* getMethodProp() const {return _prop;}
    const char* getMethodName() const {return _methodName.c_str();}

private:
    EntityMailBox*      _mailBox;
    const IProp*        _prop;
    std::string         _methodName;
        
};
#endif
