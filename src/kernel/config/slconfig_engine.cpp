#include "slkernel.h"
#include "slconfig_engine.h"
#include "slmulti_sys.h"
#include "slxml_reader.h"
#include "sltools.h"

namespace sl
{
namespace core
{
bool ConfigEngine::initialize()
{
	m_stCoreConfig.sLoopduration = 0;
	m_stCoreConfig.sTimerlooptick = 0;
	m_stCoreConfig.sAsynclooptick = 0;
	m_stCoreConfig.sAsyncThreadNum = 0;
	m_stCoreConfig.sNetlooptick = 0;

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

	char path[MAX_PATH] = {0};
	SafeSprintf(path, sizeof(path), "%s/core/server_conf.xml", sl::getAppPath());
	m_coreFile = path;

	XmlReader server_conf;
	if (!server_conf.loadXml(path)){
		SLASSERT(false, "not find core file %s", path);
		return false;
	}

	//char envirPath[MAX_PATH] = { 0 };
	m_envirPath = server_conf.root()["envir"][0].getAttributeString("path");
	m_envirPath += "/";

	m_ipcPath = server_conf.root()["ipc"][0].getAttributeString("path");
	m_ipcPath += "/";

	char moduleConfPath[MAX_PATH] = { 0 };
	SafeSprintf(moduleConfPath, sizeof(moduleConfPath), "%s/core/%s/conf.xml", sl::getAppPath(), name);
	XmlReader conf;
	if (!conf.loadXml(moduleConfPath)){
		SLASSERT(false, "can not load module core file %s", moduleConfPath);
		return false;
	}
	m_configFile = moduleConfPath;

	char logPath[MAX_PATH] = { 0 };
	SafeSprintf(logPath, sizeof(logPath), "%s/%s", sl::getAppPath(), server_conf.root()["log"][0].getAttributeString("path"));
	m_stCoreConfig.logPath = logPath;
	m_stCoreConfig.logPath += "/";
	m_stCoreConfig.logFile = string(name) + "_" + Kernel::getInstance()->getCmdArg("node_id");
	m_stCoreConfig.logFormat = server_conf.root()["log"][0].getAttributeString("format");

	m_stCoreConfig.sNetlooptick = conf.root()["net"][0].getAttributeInt32("tick");
	m_stCoreConfig.sTimerlooptick = conf.root()["timer"][0].getAttributeInt32("tick");
	m_stCoreConfig.sIpclooptick = conf.root()["ipc"][0].getAttributeInt32("tick");
	m_stCoreConfig.sLoopduration = conf.root()["loop"][0].getAttributeInt32("tick");
	if (conf.root().subNodeExist("async")){
		m_stCoreConfig.sAsynclooptick = conf.root()["async"][0].getAttributeInt32("tick");
		m_stCoreConfig.sAsyncThreadNum = conf.root()["async"][0].getAttributeInt32("threadNum");
	}
	return true;
}

bool ConfigEngine::loadModuleConfig(){
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