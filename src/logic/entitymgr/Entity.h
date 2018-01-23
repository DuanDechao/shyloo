#ifndef SL_LOGIC_CELL_ENTITY_H
#define SL_LOGIC_CELL_ENTITY_H
#include "slikernel.h"
#include "ScriptObject.h"
#include "IEntityMgr.h"
#include "slargs.h"

class IScriptDefModule;
class EntityMailBox;
class Entity: public ScriptObject{
	BASE_SCRIPT_HEADER(Entity, ScriptObject)
public:
	Entity(uint64 entityId, IScriptDefModule* pScriptModule, PyTypeObject* pyType = getScriptType(), bool isInitialised = true);
	~Entity();

    const uint64 getID(){return _id;}
	
	PyObject * onScriptGetAttribute(PyObject* attr);
	int onScriptSetAttribute(PyObject* attr, PyObject* value);

	static void installScript(const char* name);
    
    DECLARE_PY_GET_METHOD(pyGetBaseMailBox);

    inline IScriptDefModule* getScriptDefModule() const {return _pScriptModule;}

    void onRemoteMethodCall(const OArgs& args, int32& idx);

    void setBaseMailBox(const int32 logic);

private:
	uint64											_id;
	IScriptDefModule*								_pScriptModule;
    EntityMailBox*                                  _baseMailBox;
};
#endif
