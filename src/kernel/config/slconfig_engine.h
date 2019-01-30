#ifndef SL_CONFIG_ENGINE_H
#define SL_CONFIG_ENGINE_H
#include "sliconfig_engine.h"
#include "slsingleton.h"
#include <unordered_map>
#include <string>
namespace sl{
namespace core{
class ConfigEngine : public SLSingleton<ConfigEngine>{
	friend class SLSingleton<ConfigEngine>;
public:
	struct SLEnv{
		std::string _rootPath;
		std::string _resPath;
		std::string _binPath;
	};

	virtual bool ready();
	virtual bool initialize();
	virtual bool destory();
	
	void updatePaths();
	void autoSetPaths();
	virtual const char* getSysResPath();
	virtual const char* getUserResPath();
	virtual const char* getUserScriptsPath(); 
	virtual bool hasRes(const char* res);
	virtual const char* matchRes(const char* res);
	virtual int32 getResValueInt32(const char* attr);
	virtual int64 getResValueInt64(const char* attr);
	virtual const char* getResValueString(const char* attr);
	virtual bool getResValueBoolean(const char* attr);
	virtual const sCoreConfig* getCoreConfig() const {return &_stCoreConfig;}  
	virtual const sModuleConfig* getModuleConfig() const {return &_stModuleConfig;}

private:
	const char* getResValue(const char* attr);
	std::string getResValue(const char* resPath, const std::vector<std::string>& attrs);
	bool reloadCoreConfig(const char* resPath);
	bool loadCoreConfig();
	bool loadModuleConfig();

private:
	struct SLEnv	_env;
	std::vector<std::string> _resPaths;
	std::unordered_map<std::string, std::string> _resValueCache;
	sModuleConfig	_stModuleConfig;
	sCoreConfig		_stCoreConfig;
};
}
}

#endif
