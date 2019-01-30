#include "slconfig_engine.h"
#include "slstring_utils.h"
#include "slmulti_sys.h"
#include "slxml_reader.h"
#include "slkernel.h"
namespace sl{
namespace core{
bool ConfigEngine::initialize(){
	//获取引擎环境配置
	_env._rootPath = getenv("SL_ROOT") == NULL ? "" : getenv("SL_ROOT");
	_env._resPath = getenv("SL_RES_PATH") == NULL ? "" : getenv("SL_RES_PATH");
	_env._binPath = getenv("SL_BIN_PATH") == NULL ? "" : getenv("SL_BIN_PATH");

	updatePaths();
	
	if(_env._rootPath == "" || _env._resPath == "")
		autoSetPaths();

	updatePaths();
	
	if(strcmp(getSysResPath(), "") == 0 || strcmp(getUserResPath(), "") == 0 || strcmp(getUserScriptsPath(), "") == 0){
		printf("ConfigEngine::initialize: not set environment, (SL_ROOT=%s, SL_RES_PATH=%s, SL_BIN_PATH=%s) invalid\n",
				_env._rootPath.c_str(), _env._resPath.c_str(), _env._binPath.c_str());
		
		return false;
	}

	std::string resPath = matchRes("server/shyloo.xml");
	if(!reloadCoreConfig(resPath.c_str())){
		SLASSERT(false, "wtf");
		return false;
	}

	return loadCoreConfig() && loadModuleConfig();
}

bool ConfigEngine::reloadCoreConfig(const char* resPath){
	sl::XmlReader resConf;
	if (!resConf.loadXml(resPath)){
		SLASSERT(false, "cant load core file");
		return false;
	}

	const char* retValue = NULL;
	if(resConf.root().subNodeExist("parentFile")){
		std::string parentFile = matchRes(resConf.root()["parentFile"][0].getValueString());
		if(!reloadCoreConfig(parentFile.c_str()))
			return false;
	}
	return true;
}

bool ConfigEngine::loadCoreConfig(){
	_stCoreConfig.gameUpdateTick = 100;
	_stCoreConfig.maxAsyncThreadNum = 0;
	_stCoreConfig.channelWriteBufferSize = -1;
	_stCoreConfig.channelReadBufferSize = -1;

	int32 configGameUpdateHertz = getResValueInt32("gameUpdateHertz");
	_stCoreConfig.gameUpdateTick = configGameUpdateHertz ? (SECOND / configGameUpdateHertz) : _stCoreConfig.gameUpdateTick;

	const char* serverName = Kernel::getInstance()->getCmdArg("name");
	char configName[256];
	SafeSprintf(configName, 256, "%s/maxAsyncThreadNum", serverName);
	_stCoreConfig.maxAsyncThreadNum = getResValueInt32(configName);
	
	int32 configChannelWriteBufferSize = getResValueInt32("channelCommon/writeBufferSize");
	_stCoreConfig.channelWriteBufferSize = configChannelWriteBufferSize ? configChannelWriteBufferSize : _stCoreConfig.channelWriteBufferSize;
	int32 configChannelReadBufferSize = getResValueInt32("channelCommon/readBufferSize");
	_stCoreConfig.channelReadBufferSize = configChannelReadBufferSize ? configChannelReadBufferSize : _stCoreConfig.channelReadBufferSize;

	printf("ConfigEngine::loadCoreConfig: Core config gameUpdateTick:%d maxAsyncThreadNum:%d channelWriteBufferSize:%d channelReadBufferSize:%d\n", _stCoreConfig.gameUpdateTick, _stCoreConfig.maxAsyncThreadNum, _stCoreConfig.channelWriteBufferSize, _stCoreConfig.channelReadBufferSize);

	return true;
}

bool ConfigEngine::loadModuleConfig(){
	std::string modulePath = matchRes("server/shyloo_defs.xml");
	if(modulePath != "server/shyloo_defs.xml"){
		sl::CStringUtils::RepleaceAll(modulePath, "shyloo_defs.xml", "modules/");
	}
	const char* moduleName = Kernel::getInstance()->getCmdArg("name");
	char moduleFile[256];
	SafeSprintf(moduleFile, 256, "%s.xml", moduleName);
	modulePath += moduleFile;

	sl::XmlReader reader;
	if (!reader.loadXml(modulePath.c_str())){
		SLASSERT(false, "load module[%s] config failed", moduleName);
        printf("load module xml file[%s] failed\n", modulePath.c_str());
		return false;
	}

	const sl::ISLXmlNode& modules = reader.root()["module"];
	_stModuleConfig.strModulePath = reader.root().getAttributeString("path");
	for (int32 i = 0; i < modules.count(); i++){
		_stModuleConfig.vctModules.push_back(modules[i].getAttributeString("name"));
	}
	return true;
}

bool ConfigEngine::ready(){
	return true;
}

bool ConfigEngine::destory(){
	DEL this;
	return true;
}

void ConfigEngine::updatePaths(){
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

void ConfigEngine::autoSetPaths(){
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

const char* ConfigEngine::matchRes(const char* res){
	static std::string matchResStr = "";
	matchResStr = res;
	for(auto& path : _resPaths){
		std::string fPath = path + matchResStr;
		sl::CStringUtils::RepleaceAll(fPath, "\\", "/");
		sl::CStringUtils::RepleaceAll(fPath, "//", "/");

		if(access(fPath.c_str(), 0) == 0){
			matchResStr = fPath;
		}
	}

	return matchResStr.c_str();
}

const char* ConfigEngine::getSysResPath(){
	static std::string resPath = "";
	if(resPath != "")
		return resPath.c_str();

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

	return resPath.c_str();
}

const char* ConfigEngine::getUserResPath(){
	static std::string resPath = "";
	if(resPath != "")
		return resPath.c_str();

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

	return resPath.c_str();
}

const char* ConfigEngine::getUserScriptsPath(){
	static std::string path = "";
	if(path != "")
		return path.c_str();

	std::string entities_xml = "entities.xml";
	path = matchRes(entities_xml.c_str());

	if(path == entities_xml){
		entities_xml = "entities/" + entities_xml;
		path = matchRes(entities_xml.c_str());
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

	return path.c_str();
}

bool ConfigEngine::hasRes(const char* res){
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

int32 ConfigEngine::getResValueInt32(const char* attr){
	return sl::CStringUtils::StringAsInt32(getResValue(attr));
}

int64 ConfigEngine::getResValueInt64(const char* attr){
	return sl::CStringUtils::StringAsInt64(getResValue(attr));
}

const char* ConfigEngine::getResValueString(const char* attr){
	return getResValue(attr);
}

bool ConfigEngine::getResValueBoolean(const char* attr){
	return sl::CStringUtils::StringAsBoolean(getResValue(attr));
}

const char* ConfigEngine::getResValue(const char* attr){
	if(!attr){
		SLASSERT(false, "ConfigEngine::getResValue: attr == NULL");
		return "";
	}

	auto itor = _resValueCache.find(attr);
	if(itor != _resValueCache.end())
		return itor->second.c_str();

	std::string resFile = matchRes("server/shyloo.xml");
	std::vector<std::string> subAttrs;
	std::string strAttr = attr;

	subAttrs = sl::CStringUtils::splits(strAttr, "/");
	_resValueCache[attr] = getResValue(resFile.c_str(), subAttrs);
	return _resValueCache[attr].c_str();
}

std::string ConfigEngine::getResValue(const char* resPath, const std::vector<std::string>& attrs){
	sl::XmlReader resConf;
	if (!resConf.loadXml(resPath)){
		SLASSERT(false, "cant load core file");
		return NULL;
	}

	std::string retValue = "";
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

}
}

