#ifndef SL_LOGIC_ENTITY_DEF_H
#define SL_LOGIC_ENTITY_DEF_H
#include "slikernel.h"
#include "IEntityDef.h"
#include "slsingleton.h"
#include <unordered_map>
#include "slstring.h"
#include "GameDefine.h"
#include "slxml_reader.h"

#define MAX_PROP_NAME_LEN 64
#define MAX_OBJECT_NAME_LEN 64
class EntityProp;
class ScriptDefModule;
class EntityDef : public IEntityDef, public sl::SLHolder<EntityDef>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	bool loadEntitiesConfig(sl::api::IKernel* pKernel);

	bool loadAllScriptModules(sl::api::IKernel* pKernel, std::vector<PyTypeObject*>& scriptBaseTypes);

	ScriptDefModule* loadModuleDef(sl::api::IKernel* pKernel, const char* moduleName);
	ScriptDefModule* queryModuleDef(sl::api::IKernel* pKernel, const char* moduleName);

	virtual void rgsBaseScriptModule(PyTypeObject* type);
	virtual IScriptDefModule* findScriptDefModule(const char* moduleName);

private:
	typedef std::unordered_map<sl::SLString<MAX_PROP_NAME_LEN>, int32> PROP_DEFINE_MAP;
	typedef std::unordered_map<sl::SLString<MAX_OBJECT_NAME_LEN>, sl::SLString<game::MAX_PATH_LEN>> PROP_CONFIG_PATH_MAP;
	typedef std::unordered_map<sl::SLString<MAX_OBJECT_NAME_LEN>, EntityProp *> OBJECT_MODEL_MAP;
	
	typedef uint16	SCRIPT_TYPE_UID;
	typedef std::unordered_map<std::string, SCRIPT_TYPE_UID> SCRIPT_TYPE_UID_MAP;
	typedef std::vector<ScriptDefModule*> SCRIPT_DEF_MODULE_VEC;
	typedef std::unordered_map<sl::SLString<MAX_OBJECT_NAME_LEN>, ScriptDefModule *> MODEL_DEF_MAP;

	EntityDef*					_self;
	sl::api::IKernel*			_kernel;

	PROP_DEFINE_MAP				_propDefine;
	PROP_CONFIG_PATH_MAP		_propConfigsPath;
	OBJECT_MODEL_MAP			_objPropInfo;
	MODEL_DEF_MAP				_modelDefMap;
	
	std::string					_userScriptPath;
	SCRIPT_TYPE_UID_MAP			_scriptTypeMappingUType;
	SCRIPT_DEF_MODULE_VEC		_scriptModules;
	std::vector<PyTypeObject*>  _scriptBaseTypes;
};
#endif