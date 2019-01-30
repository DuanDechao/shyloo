#ifndef __SL_FRAMEWORK_NAVIGATION_H__
#define __SL_FRAMEWORK_NAVIGATION_H__
#include "slikernel.h"
#include "INavigation.h"
#include "slsingleton.h"
#include <unordered_map>
class Navigation :public INavigation, public sl::SLHolder<Navigation>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);
	
	virtual INavigationHandler* loadNavigation(const char* resPath, const std::map<int32, std::string>& params);
	virtual bool hasNavigation(const char* resPath);
	virtual bool removeNavigation(const char* resPath);
	virtual INavigationHandler* findNavigation(const char* resPath);

	void test();

	sl::api::IKernel* getKernel() {return _kernel;}

private:
	static Navigation*	s_self;
	sl::api::IKernel*	_kernel;
	std::unordered_map<std::string, INavigationHandler*> _navHandlers;
};

#endif
