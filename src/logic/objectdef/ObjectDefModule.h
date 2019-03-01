#ifndef SL_LOGIC_OBJECT_DEF_MODULE_H
#define SL_LOGIC_OBJECT_DEF_MODULE_H
#include "slikernel.h"
#include "slsingleton.h"
#include <unordered_map>
#include "slstring.h"
#include "GameDefine.h"
#include <vector>
#include "slxml_reader.h"
#include "IObjectDef.h"
#include "Attr.h"

#define MAX_PROP_NAME_LEN 64
#define MAX_OBJECT_NAME_LEN 64

struct PropDefInfo{
	PropDefInfo():
		_type(0),
		_flags(0),
		_index(0),
		_size(0),
		_extra(NULL),
		_name(""),
		_defaultVal("")
	{}

	int8	_type;
	int32	_flags;
	int32   _index;
	int32   _size;
    const void*  _extra;
	sl::SLString<MAX_PROP_NAME_LEN> _name;
	sl::SLString<256> _defaultVal;
};

class IProp;
class ISubProp;
class IObject;
class IMethod;
class RemoteEntityMethod;
class EntityMailBox;
class ObjectDefModule: public IObjectDefModule{
public:
	//entity 的数据传输特性标记

    enum RemoteMethodType{
        RMT_CLIENT = 0,
        RMT_BASE,
        RMT_CELL,
    };

	//
	ObjectDefModule(const char* moduleName, ObjectDefModule* parentModule);
	~ObjectDefModule();

	static bool initialize();

	bool loadParentModule(ObjectDefModule* parentModule);
	bool loadFrom(const sl::ISLXmlNode& root);

	virtual bool hasBase() const { return _hasBase; }
	virtual bool hasCell() const { return _hasCell; }
	virtual bool hasClient() const { return _hasClient; }
	virtual bool isPersistent() const {return _persistent;}
	inline const std::vector<PropDefInfo*>& propsDefInfo() { return _propsDefInfo; }
    inline const std::vector<PropDefInfo*>& methodsDefInfo() {return _methodsDefInfo;}
	inline void setModuleType(const int32 type) { _moduleType = type; }
	
    virtual const char* getModuleName() const { return _moduleName.c_str(); }
	virtual const int32 getModuleType() const { return _moduleType;} 
    virtual const PROPS_MAP& getProps() const {return _props;}
	virtual const PROPS_MAP& getPersistentProps() const {return _persistentProps;} 
    virtual const PROPS_MAP& getCellProps() const {return _cellProps;}
    virtual const PROPS_MAP& getClientMethods() const {return _clientMethods;}
    virtual const PROPS_MAP& getCellMethods() const {return _cellMethods;}
    virtual const PROPS_MAP& getBaseMethods() const {return _baseMethods;} 
    virtual const PROPS_IDMAP& getIdToProps() const {return _idToProps;}
    virtual const PROPS_IDMAP& getIdToClientMethods() const {return _idToClientMethods;}
    virtual const PROPS_IDMAP& getIdToCellMethods() const {return _idToCellMethods;}
    virtual const PROPS_IDMAP& getIdToBaseMethods() const {return _idToBaseMethods;} 
	virtual const IProp* getPropByUid(const int32 uid) const;

private:
	bool loadAllDefDescriptions(const char* moduleName, const sl::ISLXmlNode& root);
	bool loadDefPropertys(const char* moduleName, const sl::ISLXmlNode& root);
	bool loadDefCellMethods(const char* moduleName, const sl::ISLXmlNode& root);
	bool loadDefBaseMethods(const char* moduleName, const sl::ISLXmlNode& root);
	bool loadDefClientMethods(const char* moduleName, const sl::ISLXmlNode& root);
	bool loadDefMethods(const char* moduleName, const int8 type, const sl::ISLXmlNode& root);

    bool appendObjectProp(PropDefInfo* layout, bool isMethod = false, bool isTemp = false);
	void appendObjectSubProp(ISubProp* prop, IDataType* dataType);

private:
	//脚本类别
	typedef std::unordered_map<std::string, prop_def::ObjectDataFlag> OBJECT_FLAGS_MAP;
    static OBJECT_FLAGS_MAP             s_objectFlagMapping;
	sl::SLString<MAX_OBJECT_NAME_LEN>	_moduleName;
	int32								_moduleType;
	bool								_hasBase;
	bool								_hasCell;
	bool								_hasClient;
	bool								_persistent;
	std::vector<PropDefInfo*>			_propsDefInfo;
    std::vector<PropDefInfo*>			_methodsDefInfo;
	std::vector<PropDefInfo*>			_selfPropDefInfo;
    PROPS_MAP                           _props;
    PROPS_MAP                           _persistentProps;
    PROPS_MAP                           _cellProps;
    PROPS_IDMAP                         _idToProps;
    PROPS_MAP                           _clientMethods;
    PROPS_MAP                           _cellMethods;
    PROPS_MAP                           _baseMethods;
    PROPS_IDMAP                         _idToClientMethods;
    PROPS_IDMAP                         _idToCellMethods;
    PROPS_IDMAP                         _idToBaseMethods;
};
#endif
