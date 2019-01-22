#ifndef SL_LOGIC_SCRIPT_DEF_MODULE_H
#define SL_LOGIC_SCRIPT_DEF_MODULE_H
#include "slikernel.h"
#include "slsingleton.h"
#include <unordered_map>
#include "slstring.h"
#include "GameDefine.h"
#include <vector>
#include "slxml_reader.h"
#include "Attr.h"
#include "IObjectDef.h"
#include "slbinary_map.h"
#include "Python.h"
#define MAX_PROP_NAME_LEN 64
#define MAX_OBJECT_NAME_LEN 64
class RemoteEntityMethod;
class EntityMailBox;
class ScriptDefModule{
public:
	ScriptDefModule(const IObjectDefModule* objectDef);
	~ScriptDefModule();

	bool initialize();
	bool initDefaultCellData();
	inline bool hasBase() const { return _objectDefModule->hasBase(); }
	inline bool hasCell() const { return _objectDefModule->hasCell(); }
	inline bool hasClient() const { return _objectDefModule->hasClient(); }
	inline void setScriptType(PyTypeObject* pyType) { _scriptType = pyType; }
    virtual PyTypeObject* getScriptType() {return _scriptType;}
	virtual PyObject* scriptGetObjectAttribute(PyObject* object, PyObject* attr);
	virtual int32 scriptSetObjectAttribute(PyObject* object, PyObject* attr, PyObject* value);
    bool checkMethodArgs(IObject* object, const IProp* methodProp, PyObject* args);
	virtual PyObject* getProps() {return _propDict;}
	virtual const IProp* getPropByUid(const int32 uid) const {return _objectDefModule->getPropByUid(uid);}

	const IProp* findMethodProp(const uint16 methodIndex);
	PyObject* createArgsFromStream(IObject* object, const IProp* methodProp, sl::OBStream& stream);

	inline const char* getModuleName() const { return _objectDefModule->getModuleName(); }
	inline const int32 getModuleType() const { return _objectDefModule->getModuleType(); }
    virtual void initializeEntity(PyObject* object, PyObject* dictData);
    virtual void setDefaultCellData(PyObject* dataDict);
    virtual bool createCellDataFromStream(PyObject* object, const void* cellData, const int32 cellDataSize);
	const IProp* getMethodProp(const int8 type, PyObject* attr);

private:
	const IProp* getProp(PyObject* attr);
	void initPropDefaultValue(PyObject* object);
    void createNameSpace(PyObject* object, PyObject* dictData);
    void initializeScript(PyObject* object);

private:
	const IObjectDefModule*			_objectDefModule;
	//脚本类别
    PyTypeObject*					_scriptType;
    PyObject*						_cellPropData;
	//属性字典
	PyObject*						_propDict;
    //method Dict:
    PyObject*						_cellMethodDict;
    PyObject*						_baseMethodDict;
    PyObject*						_clientMethodDict;
	const PROPS_IDMAP&				_idToProps;
	const PROPS_IDMAP&				_idToClientMethods;
	const PROPS_IDMAP&				_idToCellMethods;
	const PROPS_IDMAP&				_idToBaseMethods;
    RemoteEntityMethod*             _remoteEntityMethod;
};
#endif
