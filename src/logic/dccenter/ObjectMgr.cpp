#include "ObjectMgr.h"
#include "slxml_reader.h"
#include "slfile_utils.h"
#include "slxml_reader.h"
#include "ObjectProp.h"
#include "MMObject.h"
#include "TableControl.h"
#include "IIdmgr.h"

bool ObjectMgr::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_nextObjTypeId = 1;

	if (!initPropDefineConfig(pKernel) || !loadObjectPropConfig(pKernel)){
		SLASSERT(false, "init config failed");
		return false;
	}

	return true;
}

bool ObjectMgr::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_idMgr, IdMgr);

	_objectStatus = _self->getPropByName("status");

	//START_TIMER(_self, 30000, TIMER_BEAT_FOREVER, 2000);
	return true;
}

bool ObjectMgr::destory(sl::api::IKernel * pKernel){
	for (auto itor = _allProps.begin(); itor != _allProps.end(); ++itor){
		if (itor->second)
			DEL itor->second;
	}
	_allProps.clear();

	for (auto itor = _objPropInfo.begin(); itor != _objPropInfo.end(); ++itor){
		if (itor->second)
			DEL itor->second;
	}
	_objPropInfo.clear();

	for (auto itor = _allObjects.begin(); itor != _allObjects.end(); ++itor){
		if (itor->second)
			DEL itor->second;
	}
	_allObjects.clear();

	for (auto itor = _tablesInfo.begin(); itor != _tablesInfo.end(); ++itor){
		if (itor->second)
			DEL itor->second;
	}
	_tablesInfo.clear();

	for (auto itor = _allTables.begin(); itor != _allTables.end(); ++itor){
		if (itor->second)
			DEL itor->second;
	}
	_allTables.clear();

	DEL this;
	return true;
}

void ObjectMgr::onTime(sl::api::IKernel* pKernel, int64 timetick){
	IObject* player = create("ddc", 1, "Player");
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
			_propDefine.insert(make_pair(name, (int32)(1 << i)));
		}
	}

	if (conf.root().subNodeExist("table")){
		const sl::ISLXmlNode& tables = conf.root()["table"];
		for (int32 i = 0; i < tables.count(); i++){
			const char* name = tables[i].getAttributeString("name");
			TableColumn* pTableColumn = NEW TableColumn();
			if (!pTableColumn->loadColumnConfig(tables[i]))
				return false;
			_tablesInfo.insert(make_pair(sl::CalcStringUniqueId(name), pTableColumn));
		}
	}
	return true;
}

bool ObjectMgr::loadObjectPropConfig(sl::api::IKernel * pKernel){
	char path[256] = { 0 };
	SafeSprintf(path, sizeof(path)-1, "%s/dccenter", pKernel->getEnvirPath());

	sl::ListFileInDirection(path, ".xml", [this](const char * name, const char * path) {
		if (_propConfigsPath.end() != _propConfigsPath.find(name)) {
			SLASSERT(false, "prop xml name repeated");
			return;
		}
		_propConfigsPath.insert(std::make_pair(name, path));
	});

	PROP_CONFIG_PATH_MAP::iterator itor = _propConfigsPath.begin();
	PROP_CONFIG_PATH_MAP::iterator itorEnd = _propConfigsPath.end();
	while (itor != itorEnd){
		createTemplate(pKernel, itor->first.c_str());
		++itor;
	}
	return true;
}

ObjectPropInfo* ObjectMgr::createTemplate(sl::api::IKernel* pKernel, const char* objectName){
	auto itor = _propConfigsPath.find(objectName);
	if (itor == _propConfigsPath.end()){
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

		propInfo = NEW ObjectPropInfo(_nextObjTypeId++, objectName, parentProp);
	}
	else{
		propInfo = NEW ObjectPropInfo(_nextObjTypeId++, objectName, nullptr);
	}

	if (!propInfo->loadFrom(propConf.root(), _propDefine)){
		DEL propInfo;
		return nullptr;
	}
	_objPropInfo[objectName] = propInfo;
	return _objPropInfo[objectName];
}

ObjectPropInfo* ObjectMgr::queryTemplate(sl::api::IKernel* pKernel, const char* objectName){
	auto itor = _objPropInfo.find(objectName);
	if (itor != _objPropInfo.end())
		return itor->second;

	return createTemplate(pKernel, objectName);
}

const IProp* ObjectMgr::setObjectProp(const char* propName, const int32 objTypeId, PropLayout* layout){
	ObjectProp * prop = nullptr;
	auto itor = _allProps.find(propName);
	if (itor != _allProps.end()){
		prop = itor->second;
	}
	else{
		prop = NEW ObjectProp(sl::CalcStringUniqueId(propName), _propConfigsPath.size());
		_allProps[propName] = prop;
	}
	prop->setLayout(objTypeId, layout);
	return prop;
}

const IProp* ObjectMgr::setObjectTempProp(const char* propName, const int32 objTypeId, PropLayout* layout){
	ObjectProp * prop = nullptr;
	auto itor = _allTempProps.find(propName);
	if (itor != _allTempProps.end()){
		prop = itor->second;
	}
	else{
		prop = NEW ObjectProp(sl::CalcStringUniqueId(propName), _propConfigsPath.size());
		_allTempProps[propName] = prop;
	}
	prop->setLayout(objTypeId, layout);
	return prop;
}

const IProp* ObjectMgr::getPropByName(const char* name) const{
	auto itor = _allProps.find(name);
	SLASSERT(itor != _allProps.end(), "wtf");
	if (itor != _allProps.end()){
		return itor->second;
	}
	return nullptr;
}

const IProp* ObjectMgr::getTempPropByName(const char* name) const{
	auto itor = _allTempProps.find(name);
	SLASSERT(itor != _allTempProps.end(), "wtf");
	if (itor != _allTempProps.end()){
		return itor->second;
	}
	return nullptr;
}

IObject* ObjectMgr::create(const char* file, const int32 line, const char* name){
	return createById(file, line, name, _idMgr->allocID());
}

IObject* ObjectMgr::createById(const char* file, const int32 line, const char* name, const uint64 id){
	auto itor = _allObjects.find(id);
	if (itor != _allObjects.end()){
		SLASSERT(false, "object[%lld] has exist!", id);
		return nullptr;
	}

	auto itor1 = _objPropInfo.find(name);
	if (itor1 == _objPropInfo.end()){
		SLASSERT(false, "object[%s]'s propInfo not exist!", name);
		return nullptr;
	}

	MMObject* object = NEW MMObject(name, itor1->second);
	object->setID(id);
	_allObjects.insert(make_pair(id, object));
	return object;
}

void ObjectMgr::recover(IObject* object){
	if (object)
		return;

	auto itor = _allObjects.find(object->getID());
	if (itor == _allObjects.end()){
		SLASSERT(false, "have no object[%lld]", object->getID());
		return;
	}
	SLASSERT(object == itor->second, "wtf");

	_allObjects.erase(itor);
	DEL object;
}

IObject* ObjectMgr::findObject(const uint64 id){
	auto itor = _allObjects.find(id);
	if (itor == _allObjects.end())
		return nullptr;

	return itor->second;
}

ITableControl* ObjectMgr::createStaticTable(const char* name){
	const int32 tableId = sl::CalcStringUniqueId(name);
	if (_allTables.find(tableId) != _allTables.end()){
		SLASSERT(false, "table %s has been created", name);
		return nullptr;
	}

	auto itor = _tablesInfo.find(tableId);
	if (itor == _tablesInfo.end()){
		SLASSERT(false, "has no table %s column info", name);
		return nullptr;
	}

	TableControl* pTableControl = NEW TableControl(tableId, itor->second);
	_allTables.insert(make_pair(tableId, pTableControl));
	return pTableControl;
}

void ObjectMgr::recoverStaticTable(ITableControl* table){
	SLASSERT(!table->getHost(), "wtf");
	_allTables.erase(((TableControl*)table)->getName());
}