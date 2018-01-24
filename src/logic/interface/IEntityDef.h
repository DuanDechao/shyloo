#ifndef _SL_INTERFACE_ENTITY_DEF_H__
#define _SL_INTERFACE_ENTITY_DEF_H__
#include "slimodule.h"
#include "python3.4m/Python.h"
class IObject;
class IProp;
class IScriptDefModule{
public:
	virtual ~IScriptDefModule() {}

	virtual PyObject* scriptGetObjectAttribute(PyObject* object, PyObject* attr) = 0;
	virtual int32 scriptSetObjectAttribute(PyObject* object, PyObject* attr, PyObject* value) = 0;
	virtual const char* getModuleName() const = 0;
    virtual PyTypeObject* getScriptType() = 0;
    virtual const IProp* getMethodProp(const int8 mailBoxType, PyObject* attr) = 0;
    virtual PyObject* createPyObject(const uint64 entityId) = 0;
    virtual void initializeEntity(PyObject* object, PyObject* dictData) = 0;
    virtual PyObject* getDefaultCellData() = 0;

    virtual bool hasBase() const  = 0;
    virtual bool hasCell() const = 0;
    virtual bool hasClient() const = 0;
};

class IEntityDef : public sl::api::IModule{
public:
	virtual ~IEntityDef() {}

	virtual void rgsBaseScriptModule(PyTypeObject* type) = 0;
	virtual IScriptDefModule* findScriptDefModule(const char* moduleName) = 0;
	virtual IScriptDefModule* findScriptDefModule(const int32 moduleType) = 0;
};
#endif
