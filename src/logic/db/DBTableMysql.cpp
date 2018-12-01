#include "DBTableMysql.h"
#include "IDB.h"
// 常规的buf长度
#define SQL_BUF 65535
#define MYSQL_ENGINE_TYPE "InnoDB"
bool DBTableItemMysql::syncItemToDB(IDBInterface* pdbi, const char* dbDataType, const char* tableName, const char* itemName,
		const int32 dataLength, enum_field_types sqlItemType, uint32 itemFlags, void* pData){
	// if(pData){
	// 	TABLE_FIELDS* pTFData = static_cast<TABLE_FIELDS*>(pData);
		
	// }
}

DBTableItemMysqlFixedDict::DBTableItemMysqlFixedDict(const char* itemName, IDataType* dataType, const char* defaultVal)
	:DBTableItem(itemName, dataType, defaultVal)
{}
	
bool DBTableItemMysqlFixedDict::initialize(){
	std::vector<std::pair<std::string, IDataType*>> keyTypes = _dataType->dictDataType();
	for(auto itor = keyTypes.begin(); itor != keyTypes.end(); itor++){
		DBTableItem* tableItem = _parentTable->createItem(itor->first.c_str(), itor->second, _defaultVal.c_str());
		tableItem->setParentTableItem(this);
		tableItem->setTableName(this->tableName());
		if(!tableItem->initialize()){
			return false;
		}

		std::pair<std::string, DBTableItem*> itemVal;
		itemVal.first = itor->first;
		itemVal.second = tableItem;
		_keyTypes.push_back(itemVal);
	}
	
	return true;
}

bool DBTableItemMysqlFixedDict::syncToDB(IDBInterface* pdbi){
	for(auto itor = keyTypes.begin(); itor != _keyTypes.end(); itor++){
		if(!itor->second->syncToDB(pdbi))
			return false;
	}
	return true;
}

bool DBTableItemMysqlArray::initialize(){
	//创建子表
	std::string tname = this->parentTable()->tableName();
	std::vector<std::string> qName;
	DBTableItem* pParentItem = this->parentTableItem();
	while(pParentItem != NULL){
		if(strlen(pParentItem->itemName()) > 0)
			qName.push_back(pParentItem->itemName());
		pParentItem = pParentItem->parentTableItem();
	}

	if(qName.size() > 0){
		for(int32 i = (int32)qName.size() -1; i >= 0; i--){
			tname += "_";
			tname += qName[i];
		}
	}

	std::string tableName = tname += "_";
	if(_itemName.size() > 0){
		tableName += _itemName;
	}
	else{
		tableName += TABLE_ARRAY_ITEM_VALUES_CONST_STR; 
	}

	DBTable* pTable = NEW DBTableMysql(tableName.c_str());
	pTable->setIsChild(true);

	std::string itemName = "";
	if(strcmp(_dataType->arrayDataType()->getName(), "FIXED_DICT") != 0)
		itemName = TABLE_ARRAY_ITEM_VALUE_CONST_STR;

	DBTableItem* pArrayTableItem = _parentTable->createItem(itemName.c_str(), _dataType->arrayDataType(), _defaultVal.c_str());
	_parentTable->dataBase()->addTable(pTable);
	pArrayTableItem->setParentTableItem(this);
	pArrayTableItem->setTableName(pTable->tableName());

	bool ret = pArrayTableItem->initialize();
	if(!ret){
		DEL pTable;
		return ret;
	}

	pTable->addItem(pArrayTableItem);
	_childTable = pTable;
	return true;
}

bool DBTableItemMysqlArray::syncToDB(IDBInterface* pdbi){

}


DBTableMysql::DBTableMysql(const char* tableName)
	:DBTable(tableName)
{}

bool DBTableMysql::initialize(){
	const IObjectDefModule* defModule = SLMODULE(ObjectDef)->findObjectDefModule(_tableName.c_str());
	if(!defModule){
		SLASSERT(false, "can't find Module %s", _tableName.c_str());
		return false;
	}

	const PROPS_MAP& persistentProp = defModule->getPersistentProps();
	for(auto prop : persistentProp){
		int8 type = prop.second->getType(defModule->getModuleName());	
		IDataType* dataType = SLMODULE(ObjectDef)->getDataType(type);
		SLASSERT(dataType, "wtf");
		const char* defaultVal = prop.second->getDefaultVal(_tableName.c_str());
		DBTableItem* pDTItem = createItem(prop.first.c_str(), dataType, defaultVal);
		if(!pDTItem->initialize())
			return false;
	}
	return true;
}

DBTableItem* DBTableMysql::createItem(const char* itemName, IDataType* dataType, const char* defaultVal){
	std::string typeName = dataType->getName();
	DBTableItem* pDTItem = NULL;
	if(typeName == "INT8"){
		if(strcmp(defaultVal, "") == 0)
			defaultVal = "0";
		pDTItem = NEW DBTableItemMysqlDigit(itemName, dataType, defaultVal);
	}
	if(typeName == "FIXED_DICT"){
		pDTItem = NEW DBTableItemMysqlFixedDict(itemName, dataType, defaultVal);
	}
	else if(typeName == "ARRAY"){
		pDTItem = NEW DBTableItemMysqlArray(itemName, dataType, defaultVal);
	}

	if(pDTItem){
		pDTItem->setParentTable(this);
		pDTItem->setTableName(this->tableName());
	}
	return pDTItem;
}

bool DBTableMysql::syncToDB(IDBInterface* pdbi){
	if(_sync)
		return true;

	char sqlBuf[SQL_BUF];
	std::string extItem = "";

	if(isChild())
		extItem = ", " TABLE_PARENTID_CONST_STR " bigint(20) unsigned NOT NULL, INDEX(" TABLE_PARENTID_CONST_STR ")";

	SafeSprintf(sqlBuf, SQL_BUF, "CREATE TABLE IF NOT EXISTS " TABLE_PERFIX "_%s "
			"(id bigint(20) unsigned AUTO_INCREMENT, PRIMARY KEY idKey (id)%s)"
			"ENGINE=" MYSQL_ENGINE_TYPE, tableName(), extItem.c_str());

	IMysqlResult* result = pdbi->execSqlSync(DB_OPT_UPDATE, sqlBuf);
	if(!result || result->getErrCode() != 0){
		ERROR_LOG("DBTableMysql::syncToDB error:%d(%s)", result->getErrCode(), result->getErrInfo());
		return false;
	}
	return true;
}

DataBaseMysql::DataBaseMysql(IDBInterface* pdbi)
	:DataBase(pdbi)
{}

bool DataBaseMysql::initialize(){
	const std::vector<const IObjectDefModule*>& allModule = SLMODULE(ObjectDef)->getAllObjectDefModule();
	for(auto defModule : allModule){
		if(!defModule->isPersistent())
			continue;

		DBTable* pTable = NEW DBTableMysql(defModule->getModuleName());
		if(!pTable->initialize()){
			DEL pTable;
			return false;
		}
		addTable(pTable);
		pTable->setDataBase(this);
	}
	return true;
}

bool DataBaseMysql::syncToDB(){
	for(auto itor : _tables){
		if(!itor->second->syncToDB(_dbInterface))
			return false;
	}
	return true;
}
