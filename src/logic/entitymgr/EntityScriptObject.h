#ifndef SL_LOGIC_SCRIPT_OBJECT_H
#define SL_LOGIC_SCRIPT_OBJECT_H
#include "Python.h"
#include <unordered_map>
#include "slmulti_sys.h"
#include "IDCCenter.h"
#include "IObjectDef.h"
#include "pyscript/scriptobject.h"
#include "ScriptDefModule.h"
#include "slbinary_stream.h"
#include "ScriptTimers.h"
class EntityScriptObject: public sl::pyscript::ScriptObject{
	BASE_SCRIPT_HREADER(EntityScriptObject, ScriptObject)
public:
	EntityScriptObject(PyTypeObject* pyType, IObject* object, ScriptDefModule* pScriptModule, bool isInitialised = false);
	~EntityScriptObject();
	
    inline void setInnerObject(IObject* object) {_innerObject = object;}
    inline IObject* getInnerObject() {return _innerObject;}
    inline ScriptDefModule* getScriptDefModule() const {return _pScriptModule;}
    void initializeEntity(PyObject* dictData){ _pScriptModule->initializeEntity(this, dictData); }
	
	PyObject * onScriptGetAttribute(PyObject* attr);
	int onScriptSetAttribute(PyObject* attr, PyObject* value);
    void onRemoteMethodCall(const sl::OBStream& stream);
	PyObject* createArgsPyObjectFromStream(const sl::OBStream& stream, const IProp* methodProp);
	inline uint64 id() const {return _innerObject->getID();}
	inline const char* getObjectName() {return _pScriptModule->getModuleName();}
	
	DECLARE_PY_GET_MOTHOD(pyGetID);
    DECLARE_PY_MOTHOD_ARG4(addTimer, float, float, const_charptr, PyObject_ptr);
    DECLARE_PY_MOTHOD_ARG1(delTimer, int32);

protected:
    IObject*					_innerObject;
    ScriptDefModule*			_pScriptModule;
	ScriptTimers				_scriptTimers;
};
#endif
