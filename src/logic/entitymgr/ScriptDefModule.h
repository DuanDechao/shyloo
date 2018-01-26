#ifndef SL_LOGIC_SCRIPT_DEF_MODULE_H
#define SL_LOGIC_SCRIPT_DEF_MODULE_H
#include "slikernel.h"
#include "slsingleton.h"
#include <unordered_map>
#include "slstring.h"
#include "GameDefine.h"
#include <vector>
#include "slxml_reader.h"
#include "IEntityDef.h"
#include "Attr.h"

#define MAX_PROP_NAME_LEN 64
#define MAX_OBJECT_NAME_LEN 64

struct PropDefInfo{
	int8	_type;
	int32	_flags;
	int32   _index;
	int32   _size;
    int64   _extra;
	sl::SLString<MAX_PROP_NAME_LEN> _name;
};

struct MethodDefInfo{
    int8   _type;
    int32  _flags;
    int32  _index;
    vector<uint8> _argsType;
    sl::SLString<MAX_PROP_NAME_LEN> _name; 
};

class IProp;
class IObject;
class IMethod;
class RemoteEntityMethod;
class EntityMailBox;
class ScriptDefModule: public IScriptDefModule{
public:
	//entity 的数据传输特性标记

    enum RemoteMethodType{
        RMT_CLIENT = 0,
        RMT_BASE,
        RMT_CELL,
    };

	//
	enum EntityDataFlagRelation{
		//所有与baseapp有关系的标志
		ENTITY_BASE_DATA_FLAGS = prop_def::EntityDataFlag::BASE | prop_def::EntityDataFlag::BASE_AND_CLIENT,
		//所有与cellapp相关的标志
		ENTITY_CELL_DATA_FLAGS = prop_def::EntityDataFlag::CELL_PUBLIC | prop_def::EntityDataFlag::CELL_PRIVATE | prop_def::EntityDataFlag::ALL_CLIENTS | prop_def::EntityDataFlag::CELL_PUBLIC_AND_OWN | prop_def::EntityDataFlag::OTHER_CLIENTS | prop_def::EntityDataFlag::OWN_CLIENT,
		//所有与client有关的标志
		ENTITY_CLIENT_DATA_FLAGS = prop_def::EntityDataFlag::BASE_AND_CLIENT | prop_def::EntityDataFlag::ALL_CLIENTS | prop_def::EntityDataFlag::CELL_PUBLIC_AND_OWN | prop_def::EntityDataFlag::OTHER_CLIENTS | prop_def::EntityDataFlag::OWN_CLIENT,
		//
	};

	ScriptDefModule(const char* moduleName, ScriptDefModule* parentModule);
	~ScriptDefModule();

	static bool initialize();

	bool loadParentModule(ScriptDefModule* parentModule);
	bool loadFrom(const sl::ISLXmlNode& root);

	virtual bool hasBase() const { return _hasBase; }
	virtual bool hasCell() const { return _hasCell; }
	virtual bool hasClient() const { return _hasClient; }
	inline const std::vector<PropDefInfo*>& propsDefInfo() { return _propsDefInfo; }
    inline const std::vector<PropDefInfo*>& methodsDefInfo() {return _methodsDefInfo;}
	inline void setScriptType(PyTypeObject* pyType) { _scriptType = pyType; }
   
    virtual PyTypeObject* getScriptType() {return _scriptType;}
	virtual PyObject* scriptGetObjectAttribute(PyObject* object, PyObject* attr);
	virtual int32 scriptSetObjectAttribute(PyObject* object, PyObject* attr, PyObject* value);
    virtual const IProp* getMethodProp(const int8 mailBoxType, PyObject* attr);
    
    bool checkMethodArgs(IObject* object, const IProp* methodProp, PyObject* args);

	virtual const char* getModuleName() const { return _moduleName.c_str(); }
    virtual PyObject* createPyObject(const uint64 entityId);
    virtual void initializeEntity(PyObject* object, PyObject* dictData);
    virtual void setDefaultCellData(PyObject* dataDict);
    virtual void addCellDataToStream(PyObject* object, PyObject* cellDataDict, sl::IBMap& dataStream);
    virtual bool createCellDataFromStream(PyObject* object, const void* cellData, const int32 cellDataSize);

private:
	bool loadAllDefDescriptions(const char* moduleName, const sl::ISLXmlNode& root);
	bool loadDefPropertys(const char* moduleName, const sl::ISLXmlNode& root);
	bool loadDefCellMethods(const char* moduleName, const sl::ISLXmlNode& root);
	bool loadDefBaseMethods(const char* moduleName, const sl::ISLXmlNode& root);
	bool loadDefClientMethods(const char* moduleName, const sl::ISLXmlNode& root);
	bool loadDefMethods(const char* moduleName, const int8 type, const sl::ISLXmlNode& root);

	const IProp* getProp(PyObject* attr);
    bool appendObjectProp(PropDefInfo* layout, bool isMethod = false, bool isTemp = false);
    
    void createNameSpace(PyObject* object, PyObject* dictData);
    void initializeScript(PyObject* object);

private:
	typedef std::unordered_map<std::string, prop_def::EntityDataFlag> ENTITY_FLAGS_MAP;
	//脚本类别
    PyTypeObject*						_scriptType;
	sl::SLString<MAX_OBJECT_NAME_LEN>	_moduleName;
	static ENTITY_FLAGS_MAP				s_entityFlagMapping;
	bool								_hasBase;
	bool								_hasCell;
	bool								_hasClient;
	std::vector<PropDefInfo*>			_propsDefInfo;
    std::vector<PropDefInfo*>           _methodsDefInfo;
	//属性字典
	PyObject*							_propDict;
    PyObject*                           _cellPropDict;
    //method Dict:
    PyObject*                           _cellMethodDict;
    PyObject*                           _baseMethodDict;
    PyObject*                           _clientMethodDict;
    RemoteEntityMethod*                 _remoteEntityMethod;
};
#endif
