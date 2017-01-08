#include "ObjectMgr.h"
#include "slxml_reader.h"
#include "slfile_utils.h"
#include "slxml_reader.h"

ObjectMgr::PROP_DEFINE_MAP ObjectMgr::s_propDefine;
ObjectMgr::PROP_CONFIG_MAP ObjectMgr::s_propConfigs;
ObjectMgr::PROP_CONFIG_PATH_MAP ObjectMgr::s_propConfigsPath;

bool ObjectMgr::initialize(sl::api::IKernel * pKernel){
	return initPropDefineConfig(pKernel) && loadObjectPropConfig(pKernel);
}

bool ObjectMgr::launched(sl::api::IKernel * pKernel){
	return true;
}

bool ObjectMgr::destory(sl::api::IKernel * pKernel){
	return true;
}

bool ObjectMgr::initPropDefineConfig(sl::api::IKernel * pKernel){
	char path[256] = { 0 };
	SafeSprintf(path, sizeof(path), "%s/object.xml", pKernel->getEnvirPath());
	sl::XmlReader conf;
	if (!conf.loadXml(path)){
		SLASSERT(false, "can not load file %s", pKernel->getEnvirPath());
		return false;
	}
	const sl::ISLXmlNode& props = conf.root()["prop"];
	for (int32 i = 0; i < props.count(); i++){
		int32 _define = 1;
		const char* name = props[i].getAttributeString("name");
		_define <<= i;
		s_propDefine.insert(make_pair(name, _define));
	}
	return true;
}

bool ObjectMgr::loadObjectPropConfig(sl::api::IKernel * pKernel){
	char path[256] = { 0 };
	SafeSprintf(path, sizeof(path)-1, "%s/dccenter", pKernel->getEnvirPath());

	std::vector<std::string> files;
	sl::getAllFilesInDir(path, files);
	for (int32 i = 0; i < (int32)files.size(); i++){
		int32 pos1 = files[i].find_last_of('/');
		int32 pos2 = files[i].find_last_of('.');
		std::string objectName(files[i].substr(pos1 + 1, pos2 - pos1-1));
		s_propConfigsPath.insert(make_pair(objectName, files[i]));
	}

	PROP_CONFIG_PATH_MAP::iterator itor = s_propConfigsPath.begin();
	PROP_CONFIG_PATH_MAP::iterator itorEnd = s_propConfigsPath.end();
	while (itor != itorEnd){
		loadObjectProp(itor->first.c_str());
		++itor;
	}
	return true;
}

ObjectPropInfo* ObjectMgr::loadObjectProp(const char* objectName){
	SLASSERT(s_propConfigsPath.find(objectName) != s_propConfigsPath.end(), "wtf");
	if (s_propConfigs.find(objectName) != s_propConfigs.end()){
		return &s_propConfigs[objectName];
	}

	sl::XmlReader propConf;
	if (!propConf.loadXml(s_propConfigsPath[objectName].c_str())){
		SLASSERT(false, "can not load file %s", s_propConfigsPath[objectName].c_str());
		return false;
	}
	if (propConf.root().hasAttribute("parent")){
		ObjectPropInfo* parentProp = loadObjectProp(propConf.root().getAttributeString("parent"));
		SLASSERT(parentProp, "wtf");
		s_propConfigs[objectName] = *parentProp;
	}

	const sl::ISLXmlNode& props = propConf.root()["prop"];
	for (int32 i = 0; i < props.count(); i++){
		const char* name = props[i].getAttributeString("name");
		const char* type = props[i].getAttributeString("type");
		int8 mask = PROP::DTYPE_UNKNOWN;
		int32 size = 0;
		if (strcmp(type,"int8") == 0){
			mask = PROP::DTYPE_INT8;
			size = sizeof(int8);
		}
		if (strcmp(type, "int16") == 0){
			mask = PROP::DTYPE_INT16;
			size = sizeof(int16);
		}
		if (strcmp(type, "int32") == 0){
			mask = PROP::DTYPE_INT32;
			size = sizeof(int32);
		}
		if (strcmp(type, "int64") == 0){
			mask = PROP::DTYPE_INT64;
			size = sizeof(int64);
		}
		if (strcmp(type, "float") == 0){
			mask = PROP::DTYPE_FLOAT;
			size = sizeof(float);
		}
		if (strcmp(type, "string") == 0){
			mask = PROP::DTYPE_STRING;
			size = props[i].getAttributeInt32("size");
		}
		
		int32 setting = 0;
		PROP_DEFINE_MAP::const_iterator itor = s_propDefine.begin();
		PROP_DEFINE_MAP::const_iterator itorEnd = s_propDefine.end();
		while (itor != itorEnd){
			const char* val = props[i].getAttributeString(itor->first.c_str());
			if (strcmp(val, "true") == 0){
				setting |= itor->second;
			}
			++itor;
		}
		int32 propId = std::hash<std::string>()(name);
		s_propConfigs[objectName].addProp(propId, name, mask, size, setting);
	}
	return &s_propConfigs[objectName];
}
