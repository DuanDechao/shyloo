#ifndef _SL_INTERFACE_OBJECT_DEF_H__
#define _SL_INTERFACE_OBJECT_DEF_H__
#include "slimodule.h"
#include "slbinary_stream.h"
#include <vector>
#include <unordered_map>
#include "sllist.h"
class IObject;
class IProp;
typedef std::unordered_map<std::string, const IProp*> PROPS_MAP;
typedef std::unordered_map<int32, const IProp*> PROPS_IDMAP;

class IArray;
class IDataType: public sl::ISLListNode {
public:
	virtual ~IDataType() {}

	/* 将脚本对象转化成C++stream 
	 * 具体可为python对象或lua对象*/
	virtual bool addScriptObject(sl::IBStream& stream, void* value) = 0;

	/*将脚本对象数据直接设置成MMObject的属性数据*/
	virtual bool addScriptObject(IObject* object, const IProp* prop, void* value) = 0;

	/**/
	virtual bool addScriptObject(IArray* array, const int32 index, void* value) = 0;

	/*从stream中创建对应DataType的脚本对象*/
	virtual void* createScriptObject(const sl::OBStream& stream) = 0;

	/*创建MMObject某个prop属性的脚本对象数据*/
	virtual void* createScriptObject(IObject* object, const IProp* prop) = 0;

	virtual void* createScriptObject(IArray* array, const int32 index) = 0;

	virtual void setUid(const uint16 id) = 0;
	virtual const uint16 getUid() const = 0;
	virtual const int8 getType() const = 0;
	virtual const char* getName() const = 0;
	virtual const int32 getSize() const = 0;
	virtual void setAliasName(const char* aliasName) = 0;
	virtual const char* getAliasName() const  = 0;
	virtual void addDataTypeInfo(sl::IBStream& stream)= 0;
	virtual void* parseDefaultStr(const char* defaultValStr) = 0;
	virtual IDataType* arrayDataType() = 0;
	virtual std::vector<std::pair<string, IDataType*>> dictDataType() = 0;
};


class IObjectDefModule{
public:
	virtual ~IObjectDefModule() {}

	virtual const char* getModuleName() const = 0;
	virtual const int32 getModuleType() const = 0; 
    virtual bool hasBase() const  = 0;
    virtual bool hasCell() const = 0;
    virtual bool hasClient() const = 0;
    virtual bool isPersistent() const = 0;
    
    virtual const PROPS_MAP& getProps() const  = 0;
	virtual const PROPS_MAP& getPersistentProps() const = 0; 
    virtual const PROPS_MAP& getClientMethods() const = 0;
    virtual const PROPS_MAP& getCellMethods() const = 0;
    virtual const PROPS_MAP& getBaseMethods() const = 0;
    virtual const PROPS_IDMAP& getIdToProps() const = 0;
    virtual const PROPS_IDMAP& getIdToClientMethods() const  = 0;
    virtual const PROPS_IDMAP& getIdToCellMethods() const = 0;
    virtual const PROPS_IDMAP& getIdToBaseMethods() const = 0;
	virtual const IProp* getPropByUid(const int32 uid) const = 0;
};

class IObjectDef : public sl::api::IModule{
public:
	virtual ~IObjectDef() {}

	virtual const IObjectDefModule* findObjectDefModule(const char* moduleName) = 0;
	virtual const IObjectDefModule* findObjectDefModule(const int32 moduleType) = 0;
    virtual const std::vector<const IObjectDefModule*>& getAllObjectDefModule() = 0;
    virtual void addAllObjectDefToStream(sl::IBStream& stream) = 0;
	virtual void addExtraDataType(const char* typeName, IDataType* dataType) = 0;
	virtual IDataType* getDataType(const char* typeName) = 0;
	virtual IDataType* getDataType(const int32 uType) = 0;
};
#endif
