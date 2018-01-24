#ifndef SL_LOGIC_BASE_H
#define SL_LOGIC_BASE_H
#include "slikernel.h"
#include "ScriptObject.h"
#include "IEntityMgr.h"
class IScriptDefModule;
class EntityMailBox;

class Base: public ScriptObject{
	BASE_SCRIPT_HEADER(Base, ScriptObject)
public:
	Base(uint64 entityId, IScriptDefModule* pScriptModule, PyTypeObject* pyType = getScriptType(), bool isInitialised = true);
	~Base();

    const uint64 getID() {return _id;}
	
	PyObject * onScriptGetAttribute(PyObject* attr);
	int onScriptSetAttribute(PyObject* attr, PyObject* value);

	static void installScript(const char* name);
    
    DECLARE_PY_GET_METHOD(pyGetCellMailBox);
    DECLARE_PY_MOTHOD_ARG1(createCellEntity, PyObject_ptr);
    DECLARE_PY_MOTHOD_ARG1(createInNewSpace, PyObject_ptr);

    void onGetCell(const int32 cellId);

    void createCellData(void);

private:
    bool installCellDataAttr(PyObject* dictData = NULL, bool installpy= true);

private:
	uint64											_id;
    IScriptDefModule*								_pScriptModule;
    EntityMailBox*                                  _cellMailBox;
    EntityMailBox*                                  _clientMailBox;
    PyObject*                                       _cellDataDict;
};
#endif
