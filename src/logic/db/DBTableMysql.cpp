#include "DBTableMysql.h"
#include "IDB.h"
// 常规的buf长度
#define SQL_BUF 65535
#define MYSQL_ENGINE_TYPE "InnoDB"
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


DBTableMysql::DBTableMysql(const char* tableName)
	:DBTable(tableName)
{}

DBTableItem* DBTableMysql::createItem(const char* itemName, IDataType* dataType, const char* defaultVal){
	std::string typeName = dataType->getName();
	DBTableItem* pDTItem = NULL;
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
	char sqlBuf[SQL_BUF];
	std::string extItem = "";

	if(isChild())
		extItem = ", " TABLE_PARENTID_CONST_STR " bigint(20) unsigned NOT NULL, INDEX(" TABLE_PARENTID_CONST_STR ")";

	SafeSprintf(sqlBuf, SQL_BUF, "CREATE TABLE IF NOT EXISTS " TABLE_PERFIX "_%s "
			"(id bigint(20) unsigned AUTO_INCREMENT, PRIMARY KEY idKey (id)%s)"
			"ENGINE=" MYSQL_ENGINE_TYPE, tableName(), extItem.c_str());

	//IDBCall* callor = CREATE_DB_CALL(SLMODULE(DB), 0, SYNC_STATE::ST_INIT);
	//callor->execRawSql(sqlBuf, DBTableMysql::syncToDBCallback);
	pdbi->execSqlSync(DB_OPT_UPDATE, sqlBuf);
	return true;
}

DataBaseMysql::DataBaseMysql(IDBInterface* pdbi)
	:DataBase(pdbi)
{}

void DataBaseMysql::initialize(){
	const std::vector<const IObjectDefModule*>& allModule = SLMODULE(ObjectDef)->getAllObjectDefModule();
	for(auto defModule : allModule){
		DBTable* pTable = NEW DBTableMysql(defModule->getModuleName());
	//	pTable->initialize();
		addTable(pTable);
		pTable->syncToDB(_dbInterface);
	}
}
