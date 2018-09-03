#ifndef SL_LOGIC_OBJECT_DEF_H
#define SL_LOGIC_OBJECT_DEF_H
#include "slikernel.h"
#include "IObjectDef.h"
#include "slsingleton.h"
#include <unordered_map>
#include "slstring.h"
#include "GameDefine.h"
#include "slxml_reader.h"
#include "slbinary_stream.h"

#define MAX_PROP_NAME_LEN 64
#define MAX_OBJECT_NAME_LEN 64
class EntityProp;
class ObjectDefModule;
class ObjectDef: public IObjectDef, public sl::SLHolder<ObjectDef>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	bool loadObjectsConfig(sl::api::IKernel* pKernel);

	ObjectDefModule* loadModuleDef(sl::api::IKernel* pKernel, const char* moduleName);
	ObjectDefModule* queryModuleDef(sl::api::IKernel* pKernel, const char* moduleName);

	virtual const IObjectDefModule* findObjectDefModule(const char* moduleName);
    virtual const IObjectDefModule* findObjectDefModule(const int32 moduleType);
    virtual const std::vector<const IObjectDefModule*>& getAllObjectDefModule() {return _allObjectModule;}
    virtual void addAllObjectDefToStream(sl::IBStream& stream);
	virtual void addExtraDataType(const char* typeName, IDataType* dataType);
	virtual IDataType* getDataType(const char* typeName);
	virtual IDataType* getDataType(const int32 uType);

private:
    bool isLoadObjectModule(ObjectDefModule* defModule);

private:
	typedef std::unordered_map<sl::SLString<MAX_PROP_NAME_LEN>, int32> PROP_DEFINE_MAP;
	typedef std::unordered_map<sl::SLString<MAX_OBJECT_NAME_LEN>, sl::SLString<game::MAX_PATH_LEN>> PROP_CONFIG_PATH_MAP;
	typedef std::unordered_map<sl::SLString<MAX_OBJECT_NAME_LEN>, EntityProp *> OBJECT_MODEL_MAP;
	
	typedef uint16	SCRIPT_TYPE_UID;
	typedef std::unordered_map<std::string, SCRIPT_TYPE_UID> SCRIPT_TYPE_UID_MAP;
	typedef std::vector<ObjectDefModule*> SCRIPT_DEF_MODULE_VEC;
	typedef std::unordered_map<sl::SLString<MAX_OBJECT_NAME_LEN>, ObjectDefModule *> MODEL_DEF_MAP;
	typedef std::unordered_map<int32, ObjectDefModule *> TYPE_MODELDEF_MAP;
	ObjectDef*					_self;
	sl::api::IKernel*			_kernel;

	PROP_DEFINE_MAP				_propDefine;
	PROP_CONFIG_PATH_MAP		_propConfigsPath;
	OBJECT_MODEL_MAP			_objPropInfo;
	MODEL_DEF_MAP				_modelDefMap;
	TYPE_MODELDEF_MAP           _typeModelDefMap;

	std::string					_userScriptPath;
	SCRIPT_TYPE_UID_MAP			_scriptTypeMappingUType;
	SCRIPT_DEF_MODULE_VEC		_scriptModules;
    std::vector<const IObjectDefModule*> _allObjectModule;

};
#endif
