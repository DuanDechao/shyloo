#include "Navigation.h"
#include "slstring_utils.h"
#include "slfile_utils.h"
#include "NavMeshHandler.h"
Navigation* Navigation::s_self = nullptr;
bool Navigation::initialize(sl::api::IKernel * pKernel){
	s_self = this;
	return true;
}

bool Navigation::launched(sl::api::IKernel * pKernel){
	test();
	return true;
}
bool Navigation::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

INavigationHandler* Navigation::loadNavigation(const char* resPath, const std::map<int32, std::string>& params){
	if(!resPath || strcmp(resPath, "") == 0)
		return nullptr;

	auto iter = _navHandlers.find(resPath);
	if(iter != _navHandlers.end())
		return iter->second;

	std::string path = Navigation::getInstance()->getKernel()->matchRes(resPath);
	if(path.size() == 0)
		return NULL;
	

	std::vector<std::string> results;
	sl::CFileUtils::ListFileInDirection(path.c_str(), ".tmx", [&results](const char * name, const char * path) {
		results.push_back(name);
	});


	INavigationHandler* pNavHandler = NULL;
	if(results.size() > 0){
	}
	else{
		results.clear();
		sl::CFileUtils::ListFileInDirection(path.c_str(), ".navmesh", [&results](const char * name, const char * path) {
			results.push_back(name);
		});

		if(results.size() == 0)
			return NULL;

		pNavHandler = NavMeshHandler::create(resPath, params);
	}

	_navHandlers[resPath] = pNavHandler;
	return pNavHandler;
}

bool Navigation::hasNavigation(const char* resPath){
	return _navHandlers.find(resPath) != _navHandlers.end();
}

bool Navigation::removeNavigation(const char* resPath){
	auto itor = _navHandlers.find(resPath);
	if(itor != _navHandlers.end()){
		//TODO 清除Handler
		DEL itor->second; 
		_navHandlers.erase(itor);
		return true;
	}
	return false;
}
	
INavigationHandler* Navigation::findNavigation(const char* resPath){
	auto iter = _navHandlers.find(resPath);
	if(iter != _navHandlers.end())
		return iter->second;
	return nullptr;
}

void Navigation::test(){
	std::map<int32, std::string> params;
	params[0] = "srv_xinshoucun_1.navmesh";
	params[1] = "srv_xinshoucun.navmesh";
	INavigationHandler* navHandler = loadNavigation("xinshoucun", params);
	Position3D center;
	center.x = 261;
	center.y = 200;
	center.z = 389; 
	std::vector<Position3D> points;
	int posNum = navHandler->findRandomPointAroundCircle(1, center, points, 256, 10);

	int32 i = 0;
}
