#include "ResMgr.h"
#include "slstring_utils.h"
#include "slmulti_sys.h"
#include "slxml_reader.h"
#include "IDebugHelper.h"
bool ResMgr::initialize(sl::api::IKernel * pKernel){
	_self = this;

	//获取引擎环境配置
	_env._rootPath = getenv("SL_ROOT") == NULL ? "" : getenv("SL_ROOT");
	_env._resPath = getenv("SL_RES_PATH") == NULL ? "" : getenv("SL_RES_PATH");
	_env._binPath = getenv("SL_BIN_PATH") == NULL ? "" : getenv("SL_BIN_PATH");

	updatePaths();
	
	if(_env._rootPath == "" || _env._resPath == "")
		autoSetPaths();

	updatePaths();
	
	TRACE_LOG("ResMgr::initialize:---- SL_ROOT:%s, SL_RES_PATH:%s SL_BIN_PATH:%s", _env._rootPath.c_str(), _env._resPath.c_str(), _env._binPath.c_str());
	if(getPySysResPath() == "" || getPyUserResPath() == "" || getPyUserScriptsPath() == ""){
		ERROR_LOG("ResMgr::initialize: not set environment, (SL_ROOT=%s, SL_RES_PATH=%s, SL_BIN_PATH=%s) invalid",
				_env._rootPath.c_str(), _env._resPath.c_str(), _env._binPath.c_str());
		
		return false;
	}

	std::string resPath = matchRes("server/shyloo.xml");
	if(!reloadCoreConfig(pKernel, resPath.c_str())){
		SLASSERT(false, "wtf");
		return false;
	}

	return true;
}

bool ResMgr::reloadCoreConfig(sl::api::IKernel* pKernel, const char* resPath){
	sl::XmlReader resConf;
	if (!resConf.loadXml(resPath)){
		SLASSERT(false, "cant load core file");
		return false;
	}

	const char* retValue = NULL;
	if(resConf.root().subNodeExist("parentFile")){
		std::string parentFile = matchRes(resConf.root()["parentFile"][0].getValueString());
		if(!reloadCoreConfig(pKernel, parentFile.c_str()))
			return false;
	}

	return pKernel->reloadCoreConfig(resPath);
}

bool ResMgr::launched(sl::api::IKernel * pKernel){
	return true;
}

bool ResMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void ResMgr::updatePaths(){
	char ch;
	if(_env._rootPath.size() > 0){
		ch = _env._rootPath.at(_env._rootPath.size() - 1);
		if(ch != '/' && ch != '\\')
			_env._rootPath += "/";
	
		sl::CStringUtils::RepleaceAll(_env._rootPath, "\\", "/");
		sl::CStringUtils::RepleaceAll(_env._rootPath, "//", "/");
	}

	if(_env._binPath.size() > 0){
		ch = _env._binPath.at(_env._binPath.size() - 1);
		if(ch != '/' && ch != '\\')
			_env._binPath += "/";

		sl::CStringUtils::RepleaceAll(_env._binPath, "\\", "/");
		sl::CStringUtils::RepleaceAll(_env._binPath, "//", "/");
	}

	std::string splitFlag = ";";
	_resPaths.clear();
	std::string resBuf = _env._resPath;
	sl::CStringUtils::Split(resBuf, splitFlag, _resPaths);
	
#ifndef SL_OS_WINDOWS
	if(_resPaths.size() < 2){
		_resPaths.clear();
		splitFlag = ":";
		sl::CStringUtils::Split(resBuf, splitFlag, _resPaths);
	}
#endif

	_env._resPath = "";
	for (auto& path : _resPaths){
		if(path.size() <= 0)
			continue;

		ch = path.at(path.size() - 1);
		if(ch != '/' && ch != '\\')
			path += '/';

		_env._resPath += path;
		_env._resPath += splitFlag;
		sl::CStringUtils::RepleaceAll(_env._resPath, "\\", "/");
		sl::CStringUtils::RepleaceAll(_env._resPath, "//", "/");
	}

	if(_env._resPath.size() > 0)
		_env._resPath.erase(_env._resPath.size() - 1);
}

void ResMgr::autoSetPaths(){
	char path[512];
	char *ret = getcwd(path, 512);
	if(ret == NULL)
		return;

	std::string spath = path;
	size_t pos1;

	sl::CStringUtils::RepleaceAll(spath, "\\", "/");
	sl::CStringUtils::RepleaceAll(spath, "//", "/");
	pos1 = spath.find("/shyloo/bin/");

	if(pos1 == std::string::npos)
		return;

	spath = spath.substr(0, pos1 + 1);
	_env._rootPath = spath;

	std::string splitFlag = ";";
#ifndef SL_OS_WINDOWS
	splitFlag = ":";
#endif
	_env._resPath = _env._rootPath + "shyloo/res/" + splitFlag + _env._rootPath + "/server/" + splitFlag + _env._rootPath + "/server/res/" + splitFlag + _env._rootPath + "/server/res/server/" + splitFlag + _env._rootPath + "/server/res/universes/";

}

std::string ResMgr::matchRes(const std::string& res){
	return matchRes(res.c_str());
}

std::string ResMgr::matchRes(const char* res){
	for(auto path : _resPaths){
		std::string fPath = path + res;
		sl::CStringUtils::RepleaceAll(fPath, "\\", "/");
		sl::CStringUtils::RepleaceAll(fPath, "//", "/");

		if(access(fPath.c_str(), 0) == 0)
			return fPath;
	}

	return res;
}

std::string ResMgr::getPySysResPath(){
	static std::string resPath = "";
	if(resPath != "")
		return resPath;

	resPath = matchRes("server/shyloo_defs.xml");
	std::vector<std::string> tmpVec;
	tmpVec = sl::CStringUtils::splits(resPath, "server/shyloo_defs.xml");

	if(tmpVec.size() > 1){
		resPath = tmpVec[0];
	}
	else{
		if(_resPaths.size() > 0)
			resPath = _resPaths[0];
	}

	return resPath;
}

std::string ResMgr::getPyUserResPath(){
	static std::string resPath = "";
	if(resPath != "")
		return resPath;

	resPath = matchRes("server/shyloo.xml");
	std::vector<std::string> tmpVec;
	tmpVec = sl::CStringUtils::splits(resPath, "server/shyloo.xml");

	if(tmpVec.size() > 1){
		resPath = tmpVec[0];
	}
	else{
		if(_resPaths.size() > 1)
			resPath = _resPaths[1];
		else if(_resPaths.size() > 0){
			resPath = _resPaths[0];
		}
	}

	return resPath;
}

std::string ResMgr::getPyUserScriptsPath(){
	static std::string path = "";
	if(path != "")
		return path;

	std::string entities_xml = "entities.xml";
	path = matchRes(entities_xml);

	if(path == entities_xml){
		entities_xml = "entities/" + entities_xml;
		path = matchRes(entities_xml);
		entities_xml = "entities.xml";
	}

	std::vector<std::string> tmpVec;
	tmpVec = sl::CStringUtils::splits(path, entities_xml);
	if(tmpVec.size() > 1){
		path = tmpVec[0];
	}
	else{
		if(_resPaths.size() > 2)
			path = _resPaths[2];
		else if(_resPaths.size() > 1)
			path = _resPaths[1];
		else if(_resPaths.size() > 0)
			path = _resPaths[0];
	}

	return path;
}

bool ResMgr::hasRes(const std::string& res){
	auto iter = _resPaths.begin();
	for(; iter != _resPaths.end(); ++iter){
		std::string fPath = ((*iter) + res);
		sl::CStringUtils::RepleaceAll(fPath, "\\", "/");
		sl::CStringUtils::RepleaceAll(fPath, "//", "/");

		if(access(fPath.c_str(), 0) == 0){
			return true;
		}
		
	}
	return false;
}

int32 ResMgr::getResValueInt32(const char* attr){
	return sl::CStringUtils::StringAsInt32(getResValue(attr));
}

int64 ResMgr::getResValueInt64(const char* attr){
	return sl::CStringUtils::StringAsInt64(getResValue(attr));
}

const char* ResMgr::getResValueString(const char* attr){
	return getResValue(attr);
}

bool ResMgr::getResValueBoolean(const char* attr){
	return sl::CStringUtils::StringAsBoolean(getResValue(attr));
}

const char* ResMgr::getResValue(const char* attr){
	auto itor = _resValueCache.find(attr);
	if(itor != _resValueCache.end())
		return itor->second.c_str();

	std::string resFile = matchRes("server/shyloo.xml");
	std::vector<std::string> subAttrs;
	std::string strAttr = attr;

	subAttrs = sl::CStringUtils::splits(strAttr, "/");
	const char* resValue = getResValue(resFile.c_str(), subAttrs);
	resValue = resValue ? resValue : "";
	_resValueCache[attr] = resValue;
	return _resValueCache[attr].c_str();
}

const char* ResMgr::getResValue(const char* resPath, const std::vector<std::string>& attrs){
	sl::XmlReader resConf;
	if (!resConf.loadXml(resPath)){
		SLASSERT(false, "cant load core file");
		return NULL;
	}

	const char* retValue = NULL;
	if(resConf.root().subNodeExist("parentFile")){
		std::string parentFile = matchRes(resConf.root()["parentFile"][0].getValueString());
		retValue = getResValue(parentFile.c_str(), attrs);
	}
	
	sl::ISLXmlNode* pXmlNode = const_cast<sl::ISLXmlNode*>(&(resConf.root()));
	for(int32 idx = 0; idx < attrs.size(); idx++){
		if(!pXmlNode->subNodeExist(attrs[idx].c_str())){
			pXmlNode = NULL;
			break;
		}
		pXmlNode = const_cast<sl::ISLXmlNode*>(&((*pXmlNode)[attrs[idx].c_str()][0]));
	}

	retValue = pXmlNode ? pXmlNode->getValueString() : retValue;

	return retValue;
}


