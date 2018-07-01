#ifndef __SL_FRAMEWORK_RESMGR_H__
#define __SL_FRAMEWORK_RESMGR_H__
#include "slikernel.h"
#include <string>
#include "IResMgr.h"
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

private:
	std::string matchRes(const char* res);

private:
	ResMgr*			_self;
	struct SLEnv	_env;
	std::vector<std::string> _resPaths;
};

#endif
