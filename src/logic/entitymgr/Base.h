#ifndef SL_LOGIC_BASE_H
#define SL_LOGIC_BASE_H
#include "slikernel.h"
#include "slscript_object.h"
#include "IEntityMgr.h"
class IScriptDefModule;
class Base: public sl::script::ScriptObject{
	BASE_SCRIPT_HEADER(Base, ScriptObject)
public:
	Base(int32 entityId, IScriptDefModule* pScriptModule, PyTypeObject* pyType = getScriptType(), bool isInitialised = true);
	~Base();
	
	PyObject * onScriptGetAttribute(PyObject* attr);
	int onScriptSetAttribute(PyObject* attr, PyObject* value);

	static void installScript(const char* name);

	void initializeScript();

private:
	int32											_id;
	IScriptDefModule*								_pScriptModule;
};
#endif