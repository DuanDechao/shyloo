#ifndef SL_LOGIC_BASE_H
#define SL_LOGIC_BASE_H
#include "slikernel.h"
#include "EntityScriptObject.h"

class IObjectDefModule;
class EntityMailBox;
class Base: public EntityScriptObject{
	BASE_SCRIPT_HREADER(Base, EntityScriptObject)
public:
	Base(IObject* object, ScriptDefModule* pScriptModule, PyTypeObject* pyType = getScriptType(), bool isInitialised = true);
	~Base();

    const uint64 getID() {return _id;}
	DECLARE_PY_GET_MOTHOD(pyGetCellMailBox);
	DECLARE_PY_GET_MOTHOD(pyGetClientMailBox);
	DECLARE_PY_GET_MOTHOD(pyGetDBID);
	DECLARE_PY_GET_MOTHOD(pyGetIsDestroyed);
    DECLARE_PY_MOTHOD_ARG1(createCellEntity, PyObject_ptr);
    DECLARE_PY_MOTHOD_ARG1(createInNewSpace, PyObject_ptr);

    void onGetCell(const int32 cellId);

    void createCellData(void);

	inline EntityMailBox* getCellMailBox() const {return _cellMailBox;}
	inline EntityMailBox* getClientMailBox() const {return _clientMailBox;}
	inline void setCellMailBox(EntityMailBox* mailBox) {_cellMailBox = mailBox;}
	inline void setClientMailBox(EntityMailBox* mailBox) {_clientMailBox = mailBox;}

protected:
    bool installCellDataAttr(PyObject* dictData = NULL, bool installpy= true);
    void remoteCreateCellEntity(int32 cellappIdx, const uint64 createToObjectId);
	void addCellDataToStream(sl::IBStream& cellDataStream);

protected:
	uint64											_id;
    EntityMailBox*                                  _cellMailBox;
    EntityMailBox*                                  _clientMailBox;
    PyObject*                                       _cellDataDict;
};
#endif
