#ifndef __SL_FRAMEWORK_RESMGR_H__
#define __SL_FRAMEWORK_RESMGR_H__
#include "slikernel.h"
#include <string>
#include "IResMgr.h"
#include <unordered_map>
class ResMgr :public IResMgr{
public:
	struct SLEnv{
		std::string _rootPath;
		std::string _resPath;
		std::string _binPath;
	};

	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);
	
	void updatePaths();
	void autoSetPaths();
	virtual std::string getPySysResPath();
	virtual std::string getPyUserResPath();
	virtual std::string getPyUserScriptsPath(); 
	virtual bool hasRes(const std::string& res);
	virtual std::string matchRes(const std::string& res);
	virtual int32 getResValueInt32(const char* attr);
	virtual int64 getResValueInt64(const char* attr);
	virtual const char* getResValueString(const char* attr);
	virtual bool getResValueBoolean(const char* attr);

private:
	std::string matchRes(const char* res);
	const char* getResValue(const char* attr);
	std::string getResValue(const char* resPath, const std::vector<std::string>& attrs);
	bool reloadCoreConfig(sl::api::IKernel* pKernel, const char* resPath);

private:
	ResMgr*			_self;
	struct SLEnv	_env;
	std::vector<std::string> _resPaths;
	std::unordered_map<std::string, std::string> _resValueCache;
};

#endif
