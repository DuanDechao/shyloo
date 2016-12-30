#include "slkernel.h"
#include "slconfig_engine.h"
#include "slmulti_sys.h"
#include "sltools.h"
#include "slxml_reader.h"

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
	return true;	
}

bool ConfigEngine::loadCoreConfig(){
	char path[MAX_PATH];
	SafeSprintf(path, sizeof(path), "%s/core/server_conf.xml", sl::getAppPath());
	m_coreFile = path;
	return true;
}

bool ConfigEngine::loadModuleConfig()
{
	const char* moduleName = Kernel::getInstance()->getCmdArg("name");
	SLASSERT(moduleName, "invaild cmd params");

	char path[MAX_PATH];
	SafeSprintf(path, sizeof(path), "%s/core/%s/module.xml", sl::getAppPath(), moduleName);
	m_moduleFile = path;

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

}
}