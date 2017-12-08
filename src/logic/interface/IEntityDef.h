#ifndef _SL_INTERFACE_ENTITY_DEF_H__
#define _SL_INTERFACE_ENTITY_DEF_H__
#include "slimodule.h"
#include "slpyscript.h"
class IObject;
class IScriptDefModule{
public:
	virtual ~IScriptDefModule() {}

	virtual PyObject* scriptGetObjectAttribute(PyObject* object, PyObject* attr) = 0;
	virtual int32 scriptSetObjectAttribute(PyObject* object, PyObject* attr, PyObject* value) = 0;
	virtual PyObject* createObject(void) = 0;
	virtual const char* getModuleName() const = 0;
	virtual IObject* getMMObject(PyObject* object) = 0;
};

class IEntityDef : public sl::api::IModule{
public:
	virtual ~IEntityDef() {}

	virtual void rgsBaseScriptModule(PyTypeObject* type) = 0;
	virtual IScriptDefModule* findScriptDefModule(const char* moduleName) = 0;
};
#endif