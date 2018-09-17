#include "slkernel.h"
#include "slconfig_engine.h"
#include "slmulti_sys.h"
#include "slxml_reader.h"
#include "sltools.h"

namespace sl
{
namespace core
{
bool ConfigEngine::initialize(){
	m_stCoreConfig.gameUpdateTick = 100;
	m_stCoreConfig.maxAsyncThreadNum = 0;
	m_stCoreConfig.channelWriteBufferSize = -1;
	m_stCoreConfig.channelReadBufferSize = -1;
	
	char coreFile[MAX_PATH] = {0};
	SafeSprintf(coreFile, sizeof(coreFile), "%s/../../res/server/shyloo_defs.xml", sl::getAppPath());
	
	const char* moduleName = Kernel::getInstance()->getCmdArg("name");
	SLASSERT(moduleName, "invaild cmd params");
	char moduleFile[MAX_PATH];
	SafeSprintf(moduleFile, sizeof(moduleFile), "%s/../../res/server/modules/%s.xml", sl::getAppPath(), moduleName);
	
	return loadCoreConfig(coreFile) && loadModuleConfig(moduleFile);
}

bool ConfigEngine::ready(){
	return true;
}

bool ConfigEngine::destory(){
	DEL this;
	return true;
}

bool ConfigEngine::loadCoreConfig(const char* path){
	XmlReader server_conf;
	if (!server_conf.loadXml(path)){
		SLASSERT(false, "not find core file %s", path);
		return false;
	}
	
	if(server_conf.root().subNodeExist("gameUpdateHertz")){
		int32 gameUpdateHertz = server_conf.root()["gameUpdateHertz"][0].getValueInt32();
		m_stCoreConfig.gameUpdateTick = SECOND / gameUpdateHertz;
	}

	if(server_conf.root().subNodeExist("maxAsyncThreadNum")){
		m_stCoreConfig.maxAsyncThreadNum = server_conf.root()["maxAsyncThreadNum"][0].getValueInt32();
	}

	if(server_conf.root().subNodeExist("channelCommon")){
		const sl::ISLXmlNode& channelCommonNode = server_conf.root()["channelCommon"][0];
		if(channelCommonNode.subNodeExist("readBufferSize")){
			m_stCoreConfig.channelReadBufferSize = channelCommonNode["readBufferSize"][0].getValueInt32();
		}

		if(channelCommonNode.subNodeExist("writeBufferSize")){
			m_stCoreConfig.channelWriteBufferSize = channelCommonNode["writeBufferSize"][0].getValueInt32();
		}
	}
	return true;
}

bool ConfigEngine::loadModuleConfig(const char* moduleFile){
	sl::XmlReader reader;
	if (!reader.loadXml(moduleFile)){
		SLASSERT(false, "load module config failed");
        printf("load module xml file[%s] failed\n", moduleFile);
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
