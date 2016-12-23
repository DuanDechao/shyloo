#include "slkernel.h"
#include "slconfig_engine.h"
#include "slmulti_sys.h"
#include "sltools.h"
#include "slxml_reader.h"

namespace sl
{
SL_SINGLETON_INIT(core::ConfigEngine);
namespace core
{
bool ConfigEngine::initialize()
{
	return true;	
}


bool ConfigEngine::loadModuleConfig()
{
	const char* moduleName = Kernel::getSingletonPtr()->getCmdArg("name");
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

}
}