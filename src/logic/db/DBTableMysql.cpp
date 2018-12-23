#include "DBTableMysql.h"
#include "IDB.h"
#include "IDCCenter.h"
// 常规的buf长度
#define SQL_BUF 65535
#define MYSQL_ENGINE_TYPE "InnoDB"
uint32 DBTableItemMysql::s_allMysqlFlags = 0;
void DBTableItemMysql::initDBItemName(const char* extraFlag){
	SafeSprintf(_dbItemName, 256, TABLE_ITEM_PREFIX"_%s%s", extraFlag, itemName());
}

bool DBTableItemMysql::syncItemToDB(IDBInterface* pdbi, const char* dbDataType, const char* tableName, const char* itemName,
		const int32 dataLength, enum_field_types sqlItemType, uint32 itemFlags, void* pData){
	 if(pData){
		IMysqlResult* mysqlResult = static_cast<IMysqlResult*>(pData);
		if(mysqlResult->columnExist(itemName)){
			int32 columnType = mysqlResult->columnType(itemName);
			uint32 columnFlags = mysqlResult->columnFlags(itemName);
			uint32 columnLength = mysqlResult->columnLength(itemName);
			if(columnType == sqlItemType && (columnFlags & s_allMysqlFlags) == itemFlags){
				if(dataLength == 0 || (sqlItemType == FIELD_TYPE_VAR_STRING ? dataLength == (columnLength / SYSTEM_CHARSET_MBMAXLEN) 
							: dataLength == columnLength))
					return true;
			}
		}
	 }
	 
	 printf("syncItemToDB: %s->%s(%s)\n", tableName, itemName, dbDataType);
	 
	 char sqlBuf[SQL_BUF];
	 SafeSprintf(sqlBuf, SQL_BUF, "ALTER TABLE `" TABLE_PREFIX "_%s` ADD `%s` %s;", tableName, itemName, dbDataType);
	 IMysqlResult* updateResult = pdbi->execSqlSync(DB_OPT_UPDATE, sqlBuf);
	 if(!updateResult || (updateResult->errCode() != 0 && updateResult->errCode() != 1060 /*Duplicate column name*/)){
		SLASSERT(false, "DBTableMysql::syncItemToDB %s failed from table(%s) failed, error:%s", itemName, tableName, updateResult->errInfo());
		updateResult->release();
		return false;
	 }

	 if(updateResult->errCode() == 1060){
		SafeSprintf(sqlBuf, SQL_BUF, "ALTER TABLE `" TABLE_PREFIX "_%s` MODIFY COLUMN `%s` %s;", tableName, itemName, dbDataType);
		IMysqlResult* modifyResult = pdbi->execSqlSync(DB_OPT_UPDATE, sqlBuf);
		if(!modifyResult || modifyResult->errCode() != 0){
			SLASSERT(false, "DBTableMysql::syncItemToDB %s failed from table(%s) failed, error:%s", itemName, tableName, modifyResult->errInfo());
			modifyResult->release();
			return false;
		}
		modifyResult->release();
	 }
	 updateResult->release();

	 return true;
}

bool DBTableItemMysqlDigit::syncToDB(IDBInterface* pdbi, void* data){
	if(_databaseLength == 0){
		sl::CStringUtils::RepleaceAll(_dbItemType, "(@DATALEN@)", "");
		return DBTableItemMysql::syncItemToDB(pdbi, _dbItemType.c_str(), _tableName.c_str(),
				dbItemName(), _databaseLength, _mysqlItemType, _flag, data);
	}

	std::string strDBLength = sl::CStringUtils::Int32AsString(_databaseLength);
	sl::CStringUtils::RepleaceAll(_dbItemType, "@DATALEN@", strDBLength);
	return DBTableItemMysql::syncItemToDB(pdbi, _dbItemType.c_str(), _tableName.c_str(),
			dbItemName(), _databaseLength, _mysqlItemType, _flag, data);
}

bool DBTableItemMysqlFixedDict::initialize(const char* defaultVal){
	std::vector<std::pair<std::string, IDataType*>> keyTypes = _dataType->dictDataType();
	for(auto itor = keyTypes.begin(); itor != keyTypes.end(); itor++){
		DBTableItem* tableItem = _parentTable->createItem(itor->first.c_str(), itor->second, defaultVal);
		tableItem->setParentTableItem(this);
		tableItem->setTableName(this->tableName());
		tableItem->setTableId(this->tableId());
		if(!tableItem->initialize(defaultVal)){
			return false;
		}

		std::pair<std::string, DBTableItem*> itemVal;
		itemVal.first = itor->first;
		itemVal.second = tableItem;
		_keyTypes.push_back(itemVal);
	}
	
	return true;
}

bool DBTableItemMysqlFixedDict::syncToDB(IDBInterface* pdbi, void* pData){
	for(auto itor = _keyTypes.begin(); itor != _keyTypes.end(); itor++){
		if(!itor->second->syncToDB(pdbi, pData)){
			SLASSERT(false, "syncToDB keyType[%s] failed", itor->first.c_str());
			return false;
		}
	}
	return true;
}

void DBTableItemMysqlFixedDict::initDBItemName(const char* extraFlag){
	for(auto itor = _keyTypes.begin(); itor != _keyTypes.end(); itor++){
		std::string newFlag = extraFlag;
		std::string dataType = itor->second->getDataType()->getName();
		if(dataType == "FIXED_DICT"){
			newFlag += itor->first + "_";
		}
		static_cast<DBTableItemMysql*>(itor->second)->initDBItemName(newFlag.c_str());
	}
}

bool DBTableItemMysqlFixedDict::isSameKey(const char* key){
	for(auto itor = _keyTypes.begin(); itor != _keyTypes.end(); itor++){
		if(itor->second->isSameKey(key))
			return true;
	}
	return false;
}

bool DBTableItemMysqlArray::initialize(const char* defaultVal){
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

	DBTable* pTable = NEW DBTableMysql(sl::CalcStringUniqueId(tableName.c_str()), tableName.c_str());
	pTable->setIsChild(true);

	std::string itemName = "";
	if(strcmp(_dataType->arrayDataType()->getName(), "FIXED_DICT") != 0)
		itemName = TABLE_ARRAY_ITEM_VALUE_CONST_STR;

	DBTableItem* pArrayTableItem = _parentTable->createItem(itemName.c_str(), _dataType->arrayDataType(), defaultVal);
	_parentTable->dataBase()->addTable(pTable);
	pArrayTableItem->setParentTableItem(this);
	pArrayTableItem->setTableName(pTable->tableName());
	pArrayTableItem->setTableId(pTable->tableId());

	bool ret = pArrayTableItem->initialize(defaultVal);
	if(!ret){
		DEL pTable;
		return ret;
	}

	pTable->addItem(pArrayTableItem);
	_childTable = pTable;
	return true;
}

void DBTableItemMysqlArray::initDBItemName(const char* extraFlag){
	if(_childTable){
		static_cast<DBTableMysql*>(_childTable)->initDBItemName();
	}
}
	
void DBTableItemMysqlVector3::initDBItemName(const char* extraFlag){
	for(int32 idx = 0; idx < 3; idx++){
		SafeSprintf(_dbItemNames[idx], 256, TABLE_ITEM_PREFIX"_%d_%s%s", idx, extraFlag, itemName());
	}
}

bool DBTableItemMysqlVector3::syncToDB(IDBInterface* pdbi, void* pData){
	for(int32 idx =0; idx < 3; idx++){
		if(!DBTableItemMysql::syncItemToDB(pdbi, _dbItemType.c_str(), _tableName.c_str(),
				_dbItemNames[idx], 0, _mysqlItemType, _flag, pData))
			return false;
	}
	return true;
}

bool DBTableItemMysqlVector3::isSameKey(const char* key){
	for(int32 idx = 0; idx < 3; idx++){
		if(strcmp(key, _dbItemNames[idx]) == 0)
			return true;
	}
	return false;
}

void DBTableItemMysqlVector2::initDBItemName(const char* extraFlag){
	for(int32 idx = 0; idx < 2; idx++){
		SafeSprintf(_dbItemNames[idx], 256, TABLE_ITEM_PREFIX"_%d_%s%s", idx, extraFlag, itemName());
	}
}

bool DBTableItemMysqlVector2::syncToDB(IDBInterface* pdbi, void* pData){
	for(int32 idx =0; idx < 2; idx++){
		if(!DBTableItemMysql::syncItemToDB(pdbi, _dbItemType.c_str(), _tableName.c_str(),
				_dbItemNames[idx], 0, _mysqlItemType, _flag, pData))
			return false;
	}
	return true;
}

bool DBTableItemMysqlVector2::isSameKey(const char* key){
	for(int32 idx = 0; idx < 2; idx++){
		if(strcmp(key, _dbItemNames[idx]) == 0)
			return true;
	}
	return false;
}

void DBTableItemMysqlVector4::initDBItemName(const char* extraFlag){
	for(int32 idx = 0; idx < 4; idx++){
		SafeSprintf(_dbItemNames[idx], 256, TABLE_ITEM_PREFIX"_%d_%s%s", idx, extraFlag, itemName());
	}
}

bool DBTableItemMysqlVector4::syncToDB(IDBInterface* pdbi, void* pData){
	for(int32 idx =0; idx < 4; idx++){
		if(!DBTableItemMysql::syncItemToDB(pdbi, _dbItemType.c_str(), _tableName.c_str(),
				_dbItemNames[idx], 0, _mysqlItemType, _flag, pData))
			return false;
	}
	return true;
}

bool DBTableItemMysqlVector4::isSameKey(const char* key){
	for(int32 idx = 0; idx < 4; idx++){
		if(strcmp(key, _dbItemNames[idx]) == 0)
			return true;
	}
	return false;
}

bool DBTableItemMysqlString::syncToDB(IDBInterface* pdbi, void* pData){
	int32 length = getDataType()->getSize();
	std::string strLength = sl::CStringUtils::Int32AsString(length);
	sl::CStringUtils::RepleaceAll(_dbItemType, "@DATALEN@", strLength);
	return DBTableItemMysql::syncItemToDB(pdbi, _dbItemType.c_str(), _tableName.c_str(),
				dbItemName(), length, _mysqlItemType, _flag, pData);
}

bool DBTableItemMysqlUnicode::syncToDB(IDBInterface* pdbi, void* pData){
	int32 length = getDataType()->getSize();
	std::string strLength = sl::CStringUtils::Int32AsString(length);
	sl::CStringUtils::RepleaceAll(_dbItemType, "@DATALEN@", strLength);
	return DBTableItemMysql::syncItemToDB(pdbi, _dbItemType.c_str(), _tableName.c_str(),
				dbItemName(), length, _mysqlItemType, _flag, pData);
}
	
bool DBTableItemMysqlPython::syncToDB(IDBInterface* pdbi, void* pData){
	return DBTableItemMysql::syncItemToDB(pdbi, _dbItemType.c_str(), _tableName.c_str(),
				dbItemName(), 0, _mysqlItemType, _flag, pData);
}

bool DBTableItemMysqlBlob::syncToDB(IDBInterface* pdbi, void* pData){
	return DBTableItemMysql::syncItemToDB(pdbi, _dbItemType.c_str(), _tableName.c_str(),
				dbItemName(), 0, _mysqlItemType, _flag, pData);
}

DBTableMysql::DBTableMysql(const int64 tableId, const char* tableName)
	:DBTable(tableId, tableName)
{}

bool DBTableMysql::initialize(){
	const IObjectDefModule* defModule = SLMODULE(ObjectDef)->findObjectDefModule(_tableName.c_str());
	if(!defModule){
		SLASSERT(false, "can't find Module %s", _tableName.c_str());
		return false;
	}

	const PROPS_MAP& persistentProp = defModule->getPersistentProps();
	for(auto prop : persistentProp){
		IDataType* dataType = (IDataType*)(prop.second->getExtra(defModule->getModuleName()));	
		SLASSERT(dataType, "wtf");
		const char* defaultVal = prop.second->getDefaultVal(_tableName.c_str());
		DBTableItem* pDTItem = createItem(prop.first.c_str(), dataType, defaultVal);
		pDTItem->setProp(prop.second);
		if(!pDTItem->initialize(defaultVal))
			return false;
		addItem(pDTItem);
	}

	initDBItemName();
	return true;
}

void DBTableMysql::initDBItemName(){
	for(auto item : _tableItems){
		//处理fixedDict字段名称的特别情况
		std::string extraFlag = "";
		std::string dataType = item.second->getDataType()->getName();
		if(dataType == "FIXED_DICT"){
			extraFlag = item.second->itemName();
			if(extraFlag.size() > 0)
				extraFlag += "_";
		}

		static_cast<DBTableItemMysql*>(item.second)->initDBItemName(extraFlag.c_str());
	}
}

DBTableItem* DBTableMysql::createItem(const char* itemName, IDataType* dataType, const char* defaultVal){
	std::string typeName = dataType->getName();
	DBTableItem* pDTItem = NULL;
	if(typeName == "INT8"){
		if(strcmp(defaultVal, "") == 0)
			defaultVal = "0";
		char sqlItemType[1024];
		SafeSprintf(sqlItemType, 1024, "tinyint(@DATALEN@) not null default %s", defaultVal);
		pDTItem = NEW DBTableItemMysqlDigit(itemName, dataType, sqlItemType, 4, NOT_NULL_FLAG, FIELD_TYPE_TINY);
	}
	else if(typeName == "INT16"){
		if(strcmp(defaultVal, "") == 0)
			defaultVal = "0";
		char sqlItemType[1024];
		SafeSprintf(sqlItemType, 1024, "smallint(@DATALEN@) not null default %s", defaultVal);
		pDTItem = NEW DBTableItemMysqlDigit(itemName, dataType, sqlItemType, 6, NOT_NULL_FLAG, FIELD_TYPE_SHORT);
	}
	else if(typeName == "INT32"){
		if(strcmp(defaultVal, "") == 0)
			defaultVal = "0";
		char sqlItemType[1024];
		SafeSprintf(sqlItemType, 1024, "int(@DATALEN@) not null default %s", defaultVal);
		pDTItem = NEW DBTableItemMysqlDigit(itemName, dataType, sqlItemType, 11, NOT_NULL_FLAG, FIELD_TYPE_LONG);
	}
	else if(typeName == "INT64"){
		if(strcmp(defaultVal, "") == 0)
			defaultVal = "0";
		char sqlItemType[1024];
		SafeSprintf(sqlItemType, 1024, "bigint(@DATALEN@) not null default %s", defaultVal);
		pDTItem = NEW DBTableItemMysqlDigit(itemName, dataType, sqlItemType, 20, NOT_NULL_FLAG, FIELD_TYPE_LONGLONG);
	}
	else if(typeName == "UINT8"){
		if(strcmp(defaultVal, "") == 0)
			defaultVal = "0";
		char sqlItemType[1024];
		SafeSprintf(sqlItemType, 1024, "tinyint(@DATALEN@) unsigned not null default %s", defaultVal);
		pDTItem = NEW DBTableItemMysqlDigit(itemName, dataType, sqlItemType, 3, NOT_NULL_FLAG | UNSIGNED_FLAG, FIELD_TYPE_TINY);
	}
	else if(typeName == "UINT16"){
		if(strcmp(defaultVal, "") == 0)
			defaultVal = "0";
		char sqlItemType[1024];
		SafeSprintf(sqlItemType, 1024, "smallint(@DATALEN@) unsigned not null default %s", defaultVal);
		pDTItem = NEW DBTableItemMysqlDigit(itemName, dataType, sqlItemType, 5, NOT_NULL_FLAG | UNSIGNED_FLAG, FIELD_TYPE_SHORT);
	}
	else if(typeName == "UINT32"){
		if(strcmp(defaultVal, "") == 0)
			defaultVal = "0";
		char sqlItemType[1024];
		SafeSprintf(sqlItemType, 1024, "int(@DATALEN@) unsigned not null default %s", defaultVal);
		pDTItem = NEW DBTableItemMysqlDigit(itemName, dataType, sqlItemType, 10, NOT_NULL_FLAG | UNSIGNED_FLAG, FIELD_TYPE_LONG);
	}
	else if(typeName == "UINT64"){
		if(strcmp(defaultVal, "") == 0)
			defaultVal = "0";
		char sqlItemType[1024];
		SafeSprintf(sqlItemType, 1024, "bigint(@DATALEN@) unsigned not null default %s", defaultVal);
		pDTItem = NEW DBTableItemMysqlDigit(itemName, dataType, sqlItemType, 20, NOT_NULL_FLAG | UNSIGNED_FLAG, FIELD_TYPE_LONGLONG);
	}
	else if(typeName == "FLOAT"){
		if(strcmp(defaultVal, "") == 0)
			defaultVal = "0";
		char sqlItemType[1024];
		SafeSprintf(sqlItemType, 1024, "float(@DATALEN@) not null default %s", defaultVal);
		pDTItem = NEW DBTableItemMysqlDigit(itemName, dataType, sqlItemType, 0, NOT_NULL_FLAG, FIELD_TYPE_FLOAT);
	}
	else if(typeName == "DOUBLE"){
		if(strcmp(defaultVal, "") == 0)
			defaultVal = "0";
		char sqlItemType[1024];
		SafeSprintf(sqlItemType, 1024, "double(@DATALEN@) not null default %s", defaultVal);
		pDTItem = NEW DBTableItemMysqlDigit(itemName, dataType, sqlItemType, 0, NOT_NULL_FLAG, FIELD_TYPE_DOUBLE);
	}
	else if(typeName == "STRING"){
		char sqlItemType[1024];
		SafeSprintf(sqlItemType, 1024, "varchar(@DATALEN@) not null default '%s'", defaultVal);
		pDTItem = NEW DBTableItemMysqlString(itemName, dataType, sqlItemType, 0, NOT_NULL_FLAG | BINARY_FLAG, FIELD_TYPE_VAR_STRING);
	}
	else if(typeName == "UNICODE"){
		char sqlItemType[1024];
		SafeSprintf(sqlItemType, 1024, "varchar(@DATALEN@) not null default '%s'", defaultVal);
		pDTItem = NEW DBTableItemMysqlUnicode(itemName, dataType, sqlItemType, 0, NOT_NULL_FLAG | BINARY_FLAG, FIELD_TYPE_VAR_STRING);
	}
	else if(typeName == "PYTHON"){
		pDTItem = NEW DBTableItemMysqlPython(itemName, dataType, "blob", 0, BINARY_FLAG | BLOB_FLAG, FIELD_TYPE_BLOB);
	}
	else if(typeName == "PY_DICT"){
		pDTItem = NEW DBTableItemMysqlPython(itemName, dataType, "blob", 0, BINARY_FLAG | BLOB_FLAG, FIELD_TYPE_BLOB);
	}
	else if(typeName == "PY_TUPLE"){
		pDTItem = NEW DBTableItemMysqlPython(itemName, dataType, "blob", 0, BINARY_FLAG | BLOB_FLAG, FIELD_TYPE_BLOB);
	}
	else if(typeName == "PY_LIST"){
		pDTItem = NEW DBTableItemMysqlPython(itemName, dataType, "blob", 0, BINARY_FLAG | BLOB_FLAG, FIELD_TYPE_BLOB);
	}
	else if(typeName == "BLOB"){
		pDTItem = NEW DBTableItemMysqlBlob(itemName, dataType, "blob", 0, BINARY_FLAG | BLOB_FLAG, FIELD_TYPE_BLOB);
	}
	else if(typeName == "FIXED_DICT"){
		pDTItem = NEW DBTableItemMysqlFixedDict(itemName, dataType, "", 0, BINARY_FLAG | BLOB_FLAG, FIELD_TYPE_BLOB);
	}
	else if(typeName == "ARRAY"){
		pDTItem = NEW DBTableItemMysqlArray(itemName, dataType, "", 0, BINARY_FLAG | BLOB_FLAG, FIELD_TYPE_BLOB);
	}
	else if(typeName == "VECTOR2"){
		pDTItem = NEW DBTableItemMysqlVector2(itemName, dataType, "float not null DEFAULT 0", 0, NOT_NULL_FLAG, FIELD_TYPE_FLOAT);
	}
	else if(typeName == "VECTOR3"){
		pDTItem = NEW DBTableItemMysqlVector3(itemName, dataType, "float not null DEFAULT 0", 0, NOT_NULL_FLAG, FIELD_TYPE_FLOAT);
	}
	else if(typeName == "VECTOR4"){
		pDTItem = NEW DBTableItemMysqlVector4(itemName, dataType, "float not null DEFAULT 0", 0, NOT_NULL_FLAG, FIELD_TYPE_FLOAT);
	}
	else if(typeName == "MAILBOX"){
		pDTItem = NEW DBTableItemMysqlMailBox(itemName, dataType, "blob", 0, BINARY_FLAG | BLOB_FLAG, FIELD_TYPE_BLOB);
	}

	if(!pDTItem){
		pDTItem = NEW DBTableItemMysqlString(itemName, dataType, "", 0, 0, FIELD_TYPE_STRING);
	}

	if(pDTItem){
		pDTItem->setParentTable(this);
		pDTItem->setTableName(this->tableName());
		pDTItem->setTableId(this->tableId());
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

	SafeSprintf(sqlBuf, SQL_BUF, "CREATE TABLE IF NOT EXISTS " TABLE_PREFIX "_%s "
			"(id bigint(20) unsigned AUTO_INCREMENT, PRIMARY KEY idKey (" TABLE_ID_CONST_STR ")%s)"
			"ENGINE=" MYSQL_ENGINE_TYPE, tableName(), extItem.c_str());

	IMysqlResult* result = pdbi->execSqlSync(DB_OPT_UPDATE, sqlBuf);
	if(!result || result->errCode() != 0){
		SLASSERT(false, "DBTableMysql::syncToDB error:%d(%s)", result->errCode(), result->errInfo());
		return false;
	}

	char fixTableName[128];
	SafeSprintf(fixTableName, 128, TABLE_PREFIX "_%s", tableName());
	IMysqlResult* fieldResult = pdbi->getTableFields(fixTableName);
	if(!fieldResult || fieldResult->errCode() != 0){
		SLASSERT(false, "DBTableMysql::syncToDB getTableField error %d(%s)", fieldResult->errCode(), fieldResult->errInfo());
		return false;
	}
	
	//同步table的field
	for(auto item : _tableItems){
		if(!item.second->syncToDB(pdbi, fieldResult)){
			SLASSERT(false, "item %s from table[%s] syncToDB failed", item.second->itemName(), tableName());
			return false;
		}
	}

	const std::vector<std::string>& columns = fieldResult->columns();

	//检查是否有需要删除的表字段
	for(auto column : columns){
		if(column == TABLE_ID_CONST_STR || column == TABLE_PARENTID_CONST_STR)
			continue;

		bool found = false;
		for(auto item : _tableItems){
			if(item.second->isSameKey(column.c_str())){
				found = true;
				break;
			}
		}

		//删除数据库字段
		if(!found){
			SafeSprintf(sqlBuf, SQL_BUF, "ALTER TABLE %s DROP COLUMN %s", fixTableName, column.c_str());
			IMysqlResult* result = pdbi->execSqlSync(DB_OPT_UPDATE, sqlBuf);
			if(!result || result->errCode() != 0){
				SLASSERT(false, "DBTableMysql::syncToDB drop column(%s) from table(%s) failed, error:%s", column.c_str(), fixTableName, result->errInfo());
				return false;
			}
		}
	}

	//同步表索引
	if(!syncIndexToDB(pdbi)){
		return false;
	}

	_sync = true;
	return true;
}

bool DBTableMysql::syncIndexToDB(IDBInterface* pdbi){
	std::vector<DBTableItem*> indexs;
	for(auto item : _tableItems){
		const IProp* prop = item.second->prop();
		if(!prop)
			continue;

		//存在普通索引或者唯一索引,index普通 identifier唯一
		const int32 flag = prop->getSetting(_tableId);
		if(flag & (prop_def::ObjectDBFlag::identifier | prop_def::ObjectDBFlag::index))
			indexs.push_back(item.second);
	}

	char sqlStr[SQL_BUF];
	SafeSprintf(sqlStr, SQL_BUF, "SHOW INDEX FROM " TABLE_PREFIX "_%s", tableName());
	IMysqlResult* result = pdbi->execSqlSync(DB_OPT_QUERY, sqlStr);
	if(!result || result->errCode() != 0){
		SLASSERT(false, "DBTableMysql::syncToIndexDB show index from table(%s) failed, error:%s", tableName(), result->errInfo());
		return false;
	}
	
	std::unordered_map<std::string, int32> currTableKeys;
	for(int32 row = 0; row < result->rowCount(); row++){
		int32 keyType = prop_def::ObjectDBFlag::identifier;
		const int8 isNoUnique = result->getDataInt8(row, "Non_unique");
		if(isNoUnique)
			keyType = prop_def::ObjectDBFlag::index;

		std::string keyName = result->getDataString(row, "Key_name");
		std::string columnName = result->getDataString(row, "Column_name");

		if(keyName == "PRIMARY" || columnName != keyName || keyName == TABLE_PARENTID_CONST_STR
				|| keyName == TABLE_ID_CONST_STR)
			continue;
		
		currTableKeys[columnName] = keyType;
	}

	bool needSync = false;
	SafeSprintf(sqlStr, SQL_BUF, "ALTER TABLE " TABLE_PREFIX "_%s ", tableName());
	for(auto itor : indexs){
		const IProp* prop = itor->prop();
		const int32 flag = prop->getSetting(_tableId);
		char fixedItemName[128];
		SafeSprintf(fixedItemName, 128, TABLE_ITEM_PREFIX "_%s", itor->itemName());
		
		auto fItor = currTableKeys.find(fixedItemName);
		if(fItor != currTableKeys.end()){
			bool isSameIndex = flag & fItor->second;
			
			//删除处理过的
			currTableKeys.erase(fItor);
			
			if(isSameIndex){
				continue;
			}
			else{
				int32 sqlStrLen = strlen(sqlStr);
				SafeSprintf(sqlStr + sqlStrLen, SQL_BUF - sqlStrLen, "DROP INDEX `%s`,", fixedItemName);
				needSync = true;
			}
			
		}

		const char* lengthStr = "";
		if(prop->getType(_tableId) == DTYPE_BLOB || prop->getType(_tableId) == DTYPE_STRING){
			char lengthStrBuf[32];
			SafeSprintf(lengthStrBuf, 32, "(%d)", prop->getSize(_tableId));
			lengthStr = lengthStrBuf;
		}

		int32 sqlStrLen = strlen(sqlStr);
		SafeSprintf(sqlStr + sqlStrLen, SQL_BUF - sqlStrLen, "ADD %s %s(%s%s),", ((flag & prop_def::ObjectDBFlag::identifier) ? "UNIQUE" : "INDEX"),
				fixedItemName, fixedItemName, lengthStr); 
		needSync = true;
	}

	//删除剩余的
	for(auto itor : currTableKeys){
		int32 sqlStrLen = strlen(sqlStr);
		SafeSprintf(sqlStr + sqlStrLen, SQL_BUF - sqlStrLen, "DROP INDEX `%s`,", itor.first.c_str());
		needSync = true;
	}

	if(!needSync)
		return true;

	IMysqlResult* updateResult = pdbi->execSqlSync(DB_OPT_UPDATE, sqlStr);
	if(!updateResult || updateResult->errCode() != 0){
		SLASSERT(false, "DBTableMysql::syncIndexToDB failed from table(%s) failed, error:%s", tableName(), updateResult->errInfo());
		return false;
	}

	return true;
}
/*
uint64 DBTableMysql::writeTable(IDBInterface* pdbi, uint64 dbid, sl::OBStream& stream, IObjectDefModule* defModule){
	while(data.getSize() > 0){
		int32 propId = 0;
		data >> propId;

		DBTableItem* pTableItem = findItem(propId);
		if(!pTableItem){
			SLASSERT(false, "cant find item[%d] from table [%s]", propId, tableName());
			return dbid;
		}

		if(!pTableItem->writeItem(pdbi, dbid, data, defModule)){
			return dbid;
		}
	}

	return dbid;
}
*/
DataBaseMysql::DataBaseMysql(IDBInterface* pdbi)
	:DataBase(pdbi)
{}

bool DataBaseMysql::initialize(){
	const std::vector<const IObjectDefModule*>& allModule = SLMODULE(ObjectDef)->getAllObjectDefModule();
	for(auto defModule : allModule){
		if(!defModule->isPersistent())
			continue;

		DBTable* pTable = NEW DBTableMysql(defModule->getModuleType(), defModule->getModuleName());
		pTable->setDataBase(this);
		if(!pTable->initialize()){
			DEL pTable;
			return false;
		}
		addTable(pTable);
	}
	return true;
}

bool DataBaseMysql::syncToDB(){
	for(auto itor : _tables){
		if(!itor.second->syncToDB(_dbInterface))
			return false;
	}
	return true;
}
