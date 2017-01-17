#include "ObjectMgr.h"
#include "slxml_reader.h"
#include "slfile_utils.h"
#include "slxml_reader.h"
#include "ObjectProp.h"
#include "MMObject.h"
#include "TableControl.h"
#include "IIdmgr.h"

ObjectMgr* ObjectMgr::s_self = nullptr;
IIdMgr* ObjectMgr::s_idMgr = nullptr;
ObjectMgr::PROP_DEFINE_MAP ObjectMgr::s_propDefine;
ObjectMgr::PROP_CONFIG_PATH_MAP ObjectMgr::s_propConfigsPath;
ObjectMgr::PROP_MAP ObjectMgr::s_allProps;
int32 ObjectMgr::s_nextObjTypeId = 1;
ObjectMgr::OBJECT_MODEL_MAP ObjectMgr::s_objPropInfo;
unordered_map<uint64, MMObject*> ObjectMgr::s_allObjects;
unordered_map<int32, TableColumn*> ObjectMgr::s_tablesInfo;
unordered_map<int32, TableControl*>  ObjectMgr::s_allTables;

bool ObjectMgr::initialize(sl::api::IKernel * pKernel){
	s_self = this;
	return initPropDefineConfig(pKernel) && loadObjectPropConfig(pKernel);
}

bool ObjectMgr::launched(sl::api::IKernel * pKernel){
	s_idMgr = (IIdMgr*)pKernel->findModule("IdMgr");
	SLASSERT(s_idMgr, "not find module IdMgr");
	START_TIMER(s_self, 30000, TIMER_BEAT_FOREVER, 2000);
	return true;
}

bool ObjectMgr::destory(sl::api::IKernel * pKernel){
	for (auto itor = s_allProps.begin(); itor != s_allProps.end(); ++itor){
		if (itor->second)
			DEL itor->second;
	}
	s_allProps.clear();

	for (auto itor = s_objPropInfo.begin(); itor != s_objPropInfo.end(); ++itor){
		if (itor->second)
			DEL itor->second;
	}
	s_objPropInfo.clear();

	for (auto itor = s_allObjects.begin(); itor != s_allObjects.end(); ++itor){
		if (itor->second)
			DEL itor->second;
	}
	s_allObjects.clear();

	for (auto itor = s_tablesInfo.begin(); itor != s_tablesInfo.end(); ++itor){
		if (itor->second)
			DEL itor->second;
	}
	s_tablesInfo.clear();

	for (auto itor = s_allTables.begin(); itor != s_allTables.end(); ++itor){
		if (itor->second)
			DEL itor->second;
	}
	s_allTables.clear();

	DEL this;
	return true;
}

void ObjectMgr::onTime(sl::api::IKernel* pKernel, int64 timetick){
	IObject* player = create("Player");
	uint64 playerId = player->getID();
	ECHO_ERROR("player id:%llu", player->getID());
}

bool ObjectMgr::initPropDefineConfig(sl::api::IKernel * pKernel){
	char path[256] = { 0 };
	SafeSprintf(path, sizeof(path), "%s/object.xml", pKernel->getEnvirPath());
	sl::XmlReader conf;
	if (!conf.loadXml(path)){
		SLASSERT(false, "can not load file %s", pKernel->getEnvirPath());
		return false;
	}
	if (conf.root().subNodeExist("prop")){
		const sl::ISLXmlNode& props = conf.root()["prop"];
		for (int32 i = 0; i < props.count(); i++){
			const char* name = props[i].getAttributeString("name");
			s_propDefine.insert(make_pair(name, (int32)(1 << i)));
		}
	}

	if (conf.root().subNodeExist("table")){
		const sl::ISLXmlNode& tables = conf.root()["table"];
		for (int32 i = 0; i < tables.count(); i++){
			const char* name = tables[i].getAttributeString("name");
			TableColumn* pTableColumn = NEW TableColumn();
			if (!pTableColumn->loadColumnConfig(tables[i]))
				return false;
			s_tablesInfo.insert(make_pair(sl::CalcStringUniqueId(name), pTableColumn));
		}
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
		createTemplate(pKernel, itor->first.c_str());
		++itor;
	}
	return true;
}

ObjectPropInfo* ObjectMgr::createTemplate(sl::api::IKernel* pKernel, const char* objectName){
	auto itor = s_propConfigsPath.find(objectName);
	if (itor == s_propConfigsPath.end()){
		SLASSERT(false, "wtf");
		return nullptr;
	}

	sl::XmlReader propConf;
	if (!propConf.loadXml(itor->second.c_str())){
		SLASSERT(false, "can not load file %s", itor->second.c_str());
		return false;
	}

	ObjectPropInfo* propInfo = nullptr;
	if (propConf.root().hasAttribute("parent")){
		ObjectPropInfo* parentProp = queryTemplate(pKernel, propConf.root().getAttributeString("parent"));
		SLASSERT(parentProp, "where is %s xml", propConf.root().getAttributeString("parent"));
		if (parentProp == nullptr)
			return nullptr;

		propInfo = NEW ObjectPropInfo(s_nextObjTypeId++, objectName, parentProp);
	}
	else{
		propInfo = NEW ObjectPropInfo(s_nextObjTypeId++, objectName, nullptr);
	}

	if (!propInfo->loadFrom(propConf.root(), s_propDefine)){
		DEL propInfo;
		return nullptr;
	}
	s_objPropInfo[objectName] = propInfo;
	return s_objPropInfo[objectName];
}

ObjectPropInfo* ObjectMgr::queryTemplate(sl::api::IKernel* pKernel, const char* objectName){
	auto itor = s_objPropInfo.find(objectName);
	if (itor != s_objPropInfo.end())
		return itor->second;

	return createTemplate(pKernel, objectName);
}

const IProp* ObjectMgr::setObjectProp(const char* propName, const int32 objTypeId, PropLayout* layout){
	ObjectProp * prop = nullptr;
	auto itor = s_allProps.find(propName);
	if (itor != s_allProps.end()){
		prop = itor->second;
	}
	else{
		prop = NEW ObjectProp(sl::CalcStringUniqueId(propName), s_propConfigsPath.size());
		s_allProps[propName] = prop;
	}
	prop->setLayout(objTypeId, layout);
	return prop;
}

const IProp* ObjectMgr::getPropByName(const char* name) const{
	auto itor = s_allProps.find(name);
	if (itor != s_allProps.end()){
		return itor->second;
	}
	return nullptr;
}

IObject* ObjectMgr::create(const char* name){
	return createById(name, s_idMgr->allocID());
}

IObject* ObjectMgr::createById(const char* name, const uint64 id){
	auto itor = s_allObjects.find(id);
	if (itor != s_allObjects.end()){
		SLASSERT(false, "object[%lld] has exist!", id);
		return nullptr;
	}

	auto itor1 = s_objPropInfo.find(name);
	if (itor1 == s_objPropInfo.end()){
		SLASSERT(false, "object[%s]'s propInfo not exist!", name);
		return nullptr;
	}

	MMObject* object = NEW MMObject(name, itor1->second);
	object->setID(id);
	s_allObjects.insert(make_pair(id, object));
	return object;
}

void ObjectMgr::recover(IObject* object){
	if (object)
		return;

	auto itor = s_allObjects.find(object->getID());
	if (itor == s_allObjects.end()){
		SLASSERT(false, "have no object[%lld]", object->getID());
		return;
	}
	SLASSERT(object == itor->second, "wtf");

	s_allObjects.erase(itor);
	DEL object;
}

const IObject* ObjectMgr::findObject(const uint64 id) const{
	auto itor = s_allObjects.find(id);
	if (itor == s_allObjects.end())
		return nullptr;

	return itor->second;
}

ITableControl* ObjectMgr::createStaticTable(const char* name){
	const int32 tableId = sl::CalcStringUniqueId(name);
	if (s_allTables.find(tableId) != s_allTables.end()){
		SLASSERT(false, "table %s has been created", name);
		return nullptr;
	}

	auto itor = s_tablesInfo.find(tableId);
	if (itor == s_tablesInfo.end()){
		SLASSERT(false, "has no table %s column info", name);
		return nullptr;
	}

	TableControl* pTableControl = NEW TableControl(tableId, itor->second);
	s_allTables.insert(make_pair(tableId, pTableControl));
	return pTableControl;
}

void ObjectMgr::recoverStaticTable(ITableControl* table){
	SLASSERT(!table->getHost(), "wtf");
	s_allTables.erase(((TableControl*)table)->getName());
}