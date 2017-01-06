#include "slkernel.h"
#include "slconfig_engine.h"
#include "slmulti_sys.h"
#include "slxml_reader.h"
#include "sltools.h"

namespace sl
{
namespace core
{
IConfigEngine* ConfigEngine::getInstance(){
	static IConfigEngine* p = nullptr;
	if (!p){
		p = NEW ConfigEngine;
		if (!p->ready()){
			SLASSERT(false, "config Engine not ready");
			DEL p;
			p = nullptr;
		}
	}
	return p;
}
bool ConfigEngine::initialize()
{
	return loadModuleConfig() && loadCoreConfig();
}

bool ConfigEngine::ready()
{
	return true;
}

bool ConfigEngine::destory()
{
	DEL this;
	return true;
}

bool ConfigEngine::loadCoreConfig(){
	const char* name = Kernel::getInstance()->getCmdArg("name");
	SLASSERT(name, "invalid command args, there is no name");

	char path[MAX_PATH];
	SafeSprintf(path, sizeof(path), "%s/core/server_conf.xml", sl::getAppPath());
	m_coreFile = path;

	XmlReader server_conf;
	if (!server_conf.loadXml(path)){
		SLASSERT(false, "not find core file %s", path);
		return false;
	}
	m_envirPath = server_conf.root()["envir"][0].getAttributeString("path");
	m_envirPath += "/";

	SafeSprintf(path, sizeof(path), "%s/core/%s/conf.xml", sl::getAppPath(), name);

	XmlReader conf;
	if (!conf.loadXml(path)){
		SLASSERT(false, "can not load module core file %s", path);
		return false;
	}

	m_configFile = path;

	m_stCoreConfig.sNetlooptick = conf.root()["net"][0].getAttributeInt32("frametick");
	m_stCoreConfig.sTimerlooptick = conf.root()["timer"][0].getAttributeInt32("tick");
	m_stCoreConfig.sLoopduration = conf.root()["loop"][0].getAttributeInt32("tick");
	return true;
}

bool ConfigEngine::loadModuleConfig()
{
	const char* moduleName = Kernel::getInstance()->getCmdArg("name");
	SLASSERT(moduleName, "invaild cmd params");

	char path[MAX_PATH];
	SafeSprintf(path, sizeof(path), "%s/core/%s/module.xml", sl::getAppPath(), moduleName);

	sl::XmlReader reader;
	if (!reader.loadXml(path)){
		SLASSERT(false, "load module config failed");
		return false;
	}

	const sl::ISLXmlNode& modules = reader.root()["module"];
	m_stModuleConfig.strModulePath = reader.root().getAttributeString("path");
	for (int32 i = 0; i < modules.count(); i++){
		m_stModuleConfig.vctModules.push_back(modules[i].getAttributeString("name"));
	}
	return true;
}

const sModuleConfig* ConfigEngine::getModuleConfig(){
	return &m_stModuleConfig;
}

const sCoreConfig* ConfigEngine::getCoreConfig(){
	return &m_stCoreConfig;
}


}
}