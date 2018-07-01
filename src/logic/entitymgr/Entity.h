#ifndef SL_LOGIC_CELL_ENTITY_H
#define SL_LOGIC_CELL_ENTITY_H
#include "slikernel.h"
#include "EntityScriptObject.h"
#include "IEntityMgr.h"
#include "slargs.h"

class IScriptDefModule;
class EntityMailBox;
class Entity: public EntityScriptObject{
	BASE_SCRIPT_HREADER(Entity, ScriptObject)
public:
	Entity(IObject* object, ScriptDefModule* pScriptModule, PyTypeObject* pyType = getScriptType(), bool isInitialised = true);
	~Entity();

    const uint64 getID(){return _id;}
    DECLARE_PY_GET_MOTHOD(pyGetBaseMailBox);
    DECLARE_PY_GET_MOTHOD(pyGetSpaceID);
	DECLARE_PY_GET_MOTHOD(pyGetID);
    void setBaseMailBox(const int32 logic);
    bool createCellDataFromStream(const void* cellData, const int32 cellDataSize);

private:
	uint64											_id;
    EntityMailBox*                                  _baseMailBox;
};
#endif
