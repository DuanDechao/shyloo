#ifndef SL_LOGIC_CELL_ENTITY_H
#define SL_LOGIC_CELL_ENTITY_H
#include "slikernel.h"
#include "EntityScriptObject.h"
#include "slargs.h"
#include "pyscript/vector3.h"
class IScriptDefModule;
class EntityMailBox;
class Entity: public EntityScriptObject{
	BASE_SCRIPT_HREADER(Entity, EntityScriptObject)
public:
	Entity(IObject* object, ScriptDefModule* pScriptModule, PyTypeObject* pyType = getScriptType(), bool isInitialised = true);
	~Entity();

    const uint64 getID(){return _id;}
    DECLARE_PY_GET_MOTHOD(pyGetBaseMailBox);
    DECLARE_PY_GET_MOTHOD(pyGetSpaceID);
    DECLARE_PY_GET_MOTHOD(pyIsWitnessed);
    DECLARE_PY_GETSET_MOTHOD(pyGetPosition, pySetPosition);
    DECLARE_PY_GETSET_MOTHOD(pyGetDirection, pySetDirection);
    DECLARE_PY_MOTHOD_ARG3(addProximity, float, float, int32);
    void setBaseMailBox(const int32 logic);
    void setClientMailBox(const int32 logic);
    bool createCellDataFromStream(const void* cellData, const int32 cellDataSize);
	inline EntityMailBox* getBaseMailBox() const {return _baseMailBox;}
	inline EntityMailBox* getClientMailBox() const {return _clientMailBox;}

private:
	uint64											_id;
    EntityMailBox*                                  _baseMailBox;
    EntityMailBox*                                  _clientMailBox;
	sl::pyscript::ScriptVector3*					_pyPosition; 
	sl::pyscript::ScriptVector3*					_pyDirection; 
};
#endif
