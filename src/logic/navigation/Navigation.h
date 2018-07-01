#ifndef __SL_FRAMEWORK_NAVIGATION_H__
#define __SL_FRAMEWORK_NAVIGATION_H__
#include "slikernel.h"
#include "INavigation.h"
#include <unordered_map>
class Navigation :public INavigation{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);
	
	virtual INavigationHandler* loadNavigation(const char* resPath, const std::map<int32, std::string>& params);
	virtual bool hasNavigation(const char* resPath);
	virtual bool removeNavigation(const char* resPath);
	virtual INavigationHandler* findNavigation(const char* resPath);

	void test();

private:
	static Navigation*	s_self;
	std::unordered_map<std::string, INavigationHandler*> _navHandlers;
};

#endif
