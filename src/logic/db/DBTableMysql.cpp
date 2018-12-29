#include "DBTableMysql.h"
#include "IDB.h"
#include "IDCCenter.h"
#include<limits.h>
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
	
bool DBTableItemMysql::readItemSql(DBContext* pContext){
	SQLCommand& sqlCommand = pContext->getSqlCommand();
	sqlCommand.select(dbItemName());
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

bool DBTableItemMysqlDigit::writeItemSql(DBContext* pContext, sl::OBStream& data){
	SQLCommand& sqlCommand = pContext->getSqlCommand();
	std::string dataType = _dataType->getName();
	if(dataType == "INT8"){
		int8 v;
		data >> v;
		sqlCommand.save(Field(dbItemName()) = v);
	}
	else if(dataType == "INT16"){
		int16 v;
		data >> v;
		sqlCommand.save(Field(dbItemName()) = v);
	}
	else if(dataType == "INT32"){
		int32 v;
		data >> v;
		sqlCommand.save(Field(dbItemName()) = v);
	}
	else if(dataType == "INT64"){
		int64 v;
		data >> v;
		sqlCommand.save(Field(dbItemName()) = v);
	}
	else if(dataType == "UINT8"){
		uint8 v;
		data >> v;
		sqlCommand.save(Field(dbItemName()) = v);
	}
	else if(dataType == "UINT16"){
		uint16 v;
		data >> v;
		sqlCommand.save(Field(dbItemName()) = v);
	}
	else if(dataType == "UINT32"){
		uint32 v;
		data >> v;
		sqlCommand.save(Field(dbItemName()) = v);
	}
	else if(dataType == "UINT64"){
		uint64 v;
		data >> v;
		sqlCommand.save(Field(dbItemName()) = v);
	}
	else if(dataType == "FLOAT"){
		float v;
		data >> v;
		sqlCommand.save(Field(dbItemName()) = v);
	}
	else if(dataType == "DOUBLE"){
		double v;
		data >> v;
		sqlCommand.save(Field(dbItemName()) = v);
	}
	return true;
}

bool DBTableItemMysqlDigit::addStream(DBContext* pContext, sl::IBStream& data){
	IMysqlResult* result = pContext->dbResult();
	SLASSERT(result, "wtf");

	int32 dbRowIdx = pContext->dbRowIndex();
	SLASSERT(dbRowIdx < result->rowCount(), "wtf");

	std::string dataType = _dataType->getName();
	if(dataType == "INT8"){
		data << result->getDataInt8(dbRowIdx, dbItemName());
	}
	else if(dataType == "INT16"){
		data << result->getDataInt16(dbRowIdx, dbItemName());
	}
	else if(dataType == "INT32"){
		data << result->getDataInt32(dbRowIdx, dbItemName());
	}
	else if(dataType == "INT64"){
		data << result->getDataInt64(dbRowIdx, dbItemName());
	}
	else if(dataType == "UINT8"){
		data << (uint8)result->getDataInt8(dbRowIdx, dbItemName());
	}
	else if(dataType == "UINT16"){
		data << (uint16)result->getDataInt16(dbRowIdx, dbItemName());
	}
	else if(dataType == "UINT32"){
		data << (uint32)result->getDataInt32(dbRowIdx, dbItemName());
	}
	else if(dataType == "UINT64"){
		data << (uint64)result->getDataInt64(dbRowIdx, dbItemName());
	}
	else if(dataType == "FLOAT"){
		data << (float)result->getDataFloat(dbRowIdx, dbItemName());
	}
	else if(dataType == "DOUBLE"){
		data << (double)result->getDataFloat(dbRowIdx, dbItemName());
	}
	return true;
}

bool DBTableItemMysqlDigit::makeTestData(sl::IBStream& data){
	if(this->uType() > 0)
		data << this->uType(); 

	std::string dataType = _dataType->getName();
	int32 rand = sl::getRandom(0, INT_MAX);
	if(dataType == "INT8"){
		int8 v = rand;
		data << v;
	}
	else if(dataType == "INT16"){
		int16 v = rand;
		data << v;
	}
	else if(dataType == "INT32"){
		int32 v = rand;
		data << v;
	}
	else if(dataType == "INT64"){
		int64 v = rand;
		data << v;
	}
	else if(dataType == "UINT8"){
		uint8 v = rand;
		data << v;
	}
	else if(dataType == "UINT16"){
		uint16 v = rand;
		data << v;
	}
	else if(dataType == "UINT32"){
		uint32 v = rand;
		data << v;
	}
	else if(dataType == "UINT64"){
		uint64 v = rand;
		data << v;
	}
	else if(dataType == "FLOAT"){
		float v = rand;
		data << v;
	}
	else if(dataType == "DOUBLE"){
		double v = rand;
		data << v;
	}
	return true;
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

bool DBTableItemMysqlFixedDict::writeItemSql(DBContext* pContext, sl::OBStream& data){
	for(auto itor = _keyTypes.begin(); itor != _keyTypes.end(); itor++){
		if(!itor->second->writeItemSql(pContext, data))
			return false;
	}
	return true;
}

bool DBTableItemMysqlFixedDict::readItemSql(DBContext* pContext){
	SQLCommand& sqlCommand = pContext->getSqlCommand();
	for(auto itor = _keyTypes.begin(); itor != _keyTypes.end(); itor++){
		if(!itor->second->readItemSql(pContext))
			return false;
	}
	return true;
}

bool DBTableItemMysqlFixedDict::addStream(DBContext* pContext, sl::IBStream& data){
	for(auto itor = _keyTypes.begin(); itor != _keyTypes.end(); itor++){
		if(!itor->second->addStream(pContext, data))
			return false;
	}
	return true;
}

bool DBTableItemMysqlFixedDict::makeTestData(sl::IBStream& data){
	if(this->uType() > 0)
		data << this->uType();

	for(auto itor = _keyTypes.begin(); itor != _keyTypes.end(); itor++){
		if(!itor->second->makeTestData(data))
			return false;
	}
	return true;
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
	_parentTable->addSubTable(pTable);

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

bool DBTableItemMysqlArray::writeItemSql(DBContext* pContext, sl::OBStream& data){
	int32 size = 0;
	data >> size;
	SLASSERT(_childTable, "wtf");

	if(_childTable){
		char fixedTableName[256];
		SafeSprintf(fixedTableName, 256, TABLE_PREFIX "_%s", _childTable->tableName());

		//清空
		if(size <= 0){
			if(pContext->dbid() <= 0)
				return true;

			//清空_childTable->tableName()
			pContext->addSubContext(_childTable->tableName(), NULL);
		}
		else{
			for(int32 idx = 0; idx < size; idx++){
				DataBaseMysql* database = static_cast<DataBaseMysql*>(_parentTable->dataBase());
				SQLCommand& sqlCommand = database->createSqlCommand();
				DBContext* newDBContext = NEW DBContext(_childTable->tableName(), sqlCommand, 0);
				sqlCommand.table(fixedTableName);
				if(!static_cast<DBTableMysql*>(_childTable)->writeItemSql(newDBContext, data))
					return false;

				pContext->addSubContext(_childTable->tableName(), newDBContext);
			}
		}
	}
	return true;
}

bool DBTableItemMysqlArray::readItemSql(DBContext* pContext){
	if(_childTable){
		SQLCommand& newSqlCommand = static_cast<DataBaseMysql*>(_parentTable->dataBase())->createSqlCommand();
		DBContext* pSubContext = NEW DBContext(_childTable->tableName(), newSqlCommand, 0);
		pContext->addSubContext(_childTable->tableName(), pSubContext);
		static_cast<DBTableMysql*>(_childTable)->readItemSql(pContext);
	}
	return true;
}

bool DBTableItemMysqlArray::addStream(DBContext* pContext, sl::IBStream& data){
	DBContext::SUB_CONTEXTS& subContexts = pContext->subContexts();
	auto contextItor = subContexts.find(_childTable->tableName());
	SLASSERT(contextItor != subContexts.end(), "wtf");

	for(auto cItor : contextItor->second){
		cItor->setParentDBId(pContext->dbid());
		if(static_cast<DBTableMysql*>(_childTable)->readDataFromDB(_childTable->dataBase()->getDBInterface(), cItor, data))
			return false;
	}
	return true;
}

bool DBTableItemMysqlArray::makeTestData(sl::IBStream& data){
	if(this->uType() > 0)
		data << this->uType();

	int32 rand = sl::getRandom(0, 100);
	data << rand;
	for(int32 idx = 0; idx < rand; idx++){
		if(_childTable){
			_childTable->makeTestData(data);
		}
	}
	return true;
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

bool DBTableItemMysqlVector3::writeItemSql(DBContext* pContext, sl::OBStream& data){
	SQLCommand& sqlCommand = pContext->getSqlCommand();
	for(int32 idx = 0; idx < 3; idx++){
		float v;
		data >> v;
		sqlCommand.save(Field(_dbItemNames[idx]) = v);
	}
	return true;
}

bool DBTableItemMysqlVector3::readItemSql(DBContext* pContext){
	SQLCommand& sqlCommand = pContext->getSqlCommand();
	for(int32 idx =0; idx < 3; idx++){
		sqlCommand.select(_dbItemNames[idx]);
	}
	return true;
}

bool DBTableItemMysqlVector3::addStream(DBContext* pContext, sl::IBStream& data){
	IMysqlResult* result = pContext->dbResult();
	SLASSERT(result, "wtf");

	int32 dbRowIdx = pContext->dbRowIndex();
	SLASSERT(dbRowIdx < result->rowCount(), "wtf");

	for(int32 idx = 0; idx < 3; idx++){
		data << result->getDataFloat(dbRowIdx, _dbItemNames[idx]);
	}
	return true;
}

bool DBTableItemMysqlVector3::makeTestData(sl::IBStream& data){
	if(this->uType() > 0)
		data << this->uType();

	for(int32 idx = 0; idx < 3; idx++){
		float rand = sl::getRandom(0, INT_MAX);
		data << rand;
	}
	return true;
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

bool DBTableItemMysqlVector2::writeItemSql(DBContext* pContext, sl::OBStream& data){
	SQLCommand& sqlCommand = pContext->getSqlCommand();
	for(int32 idx = 0; idx < 2; idx++){
		float v;
		data >> v;
		sqlCommand.save(Field(_dbItemNames[idx]) = v);
	}
	return true;
}

bool DBTableItemMysqlVector2::readItemSql(DBContext* pContext){
	SQLCommand& sqlCommand = pContext->getSqlCommand();
	for(int32 idx = 0; idx < 2; idx++){
		sqlCommand.select(_dbItemNames[idx]);
	}
	return true;
}

bool DBTableItemMysqlVector2::addStream(DBContext* pContext, sl::IBStream& data){
	IMysqlResult* result = pContext->dbResult();
	SLASSERT(result, "wtf");

	int32 dbRowIdx = pContext->dbRowIndex();
	SLASSERT(dbRowIdx < result->rowCount(), "wtf");

	for(int32 idx = 0; idx < 2; idx++){
		data << result->getDataFloat(dbRowIdx, _dbItemNames[idx]);
	}
	return true;
}

bool DBTableItemMysqlVector2::makeTestData(sl::IBStream& data){
	if(this->uType() > 0)
		data << this->uType();

	for(int32 idx = 0; idx < 2; idx++){
		float rand = sl::getRandom(0, INT_MAX);
		data << rand;
	}
	return true;
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

bool DBTableItemMysqlVector4::readItemSql(DBContext* pContext){
	SQLCommand& sqlCommand = pContext->getSqlCommand();
	for(int32 idx = 0; idx < 4; idx++){
		sqlCommand.select(_dbItemNames[idx]);
	}
	return true;
}

bool DBTableItemMysqlVector4::writeItemSql(DBContext* pContext, sl::OBStream& data){
	SQLCommand& sqlCommand = pContext->getSqlCommand();
	for(int32 idx = 0; idx < 4; idx++){
		float v;
		data >> v;
		sqlCommand.save(Field(_dbItemNames[idx]) = v);
	}
	return true;
}

bool DBTableItemMysqlVector4::addStream(DBContext* pContext, sl::IBStream& data){
	IMysqlResult* result = pContext->dbResult();
	SLASSERT(result, "wtf");

	int32 dbRowIdx = pContext->dbRowIndex();
	SLASSERT(dbRowIdx < result->rowCount(), "wtf");

	for(int32 idx = 0; idx < 4; idx++){
		data << result->getDataFloat(dbRowIdx, _dbItemNames[idx]);
	}
	return true;
}

bool DBTableItemMysqlVector4::makeTestData(sl::IBStream& data){
	if(this->uType() > 0)
		data << this->uType();

	for(int32 idx = 0; idx < 4; idx++){
		float rand = sl::getRandom(0, INT_MAX);
		data << rand;
	}
	return true;
}

bool DBTableItemMysqlString::syncToDB(IDBInterface* pdbi, void* pData){
	int32 length = getDataType()->getSize();
	std::string strLength = sl::CStringUtils::Int32AsString(length);
	sl::CStringUtils::RepleaceAll(_dbItemType, "@DATALEN@", strLength);
	return DBTableItemMysql::syncItemToDB(pdbi, _dbItemType.c_str(), _tableName.c_str(),
				dbItemName(), length, _mysqlItemType, _flag, pData);
}

bool DBTableItemMysqlString::writeItemSql(DBContext* pContext, sl::OBStream& data){
	SQLCommand& sqlCommand = pContext->getSqlCommand();
	const char* v = nullptr;
	data >> v;
	sqlCommand.save(Field(dbItemName()) = v);
	return true;
}

bool DBTableItemMysqlString::addStream(DBContext* pContext, sl::IBStream& data){
	IMysqlResult* result = pContext->dbResult();
	SLASSERT(result, "wtf");

	int32 dbRowIdx = pContext->dbRowIndex();
	SLASSERT(dbRowIdx < result->rowCount(), "wtf");

	data << result->getDataString(dbRowIdx, dbItemName());
	return true;
}

bool DBTableItemMysqlString::makeTestData(sl::IBStream& data){
	if(this->uType() > 0)
		data << this->uType();
	
	const char* test = "dsfsdfasdaaaaaaaaaaaaaaaaaaa";
	data << test;
	return true;
}

bool DBTableItemMysqlUnicode::syncToDB(IDBInterface* pdbi, void* pData){
	int32 length = getDataType()->getSize();
	std::string strLength = sl::CStringUtils::Int32AsString(length);
	sl::CStringUtils::RepleaceAll(_dbItemType, "@DATALEN@", strLength);
	return DBTableItemMysql::syncItemToDB(pdbi, _dbItemType.c_str(), _tableName.c_str(),
				dbItemName(), length, _mysqlItemType, _flag, pData);
}

bool DBTableItemMysqlUnicode::writeItemSql(DBContext* pContext, sl::OBStream& data){
	SQLCommand& sqlCommand = pContext->getSqlCommand();
	const char* v = nullptr;
	data >> v;
	sqlCommand.save(Field(dbItemName()) = v);
	return true;
}

bool DBTableItemMysqlUnicode::addStream(DBContext* pContext, sl::IBStream& data){
	IMysqlResult* result = pContext->dbResult();
	SLASSERT(result, "wtf");

	int32 dbRowIdx = pContext->dbRowIndex();
	SLASSERT(dbRowIdx < result->rowCount(), "wtf");

	data << result->getDataString(dbRowIdx, dbItemName());
	return true;
}

bool DBTableItemMysqlUnicode::makeTestData(sl::IBStream& data){
	if(this->uType() > 0)
		data << this->uType();
	
	const char* test = "aaaaaaaaaa";
	data << test;
	return true;
}
	
bool DBTableItemMysqlPython::syncToDB(IDBInterface* pdbi, void* pData){
	return DBTableItemMysql::syncItemToDB(pdbi, _dbItemType.c_str(), _tableName.c_str(),
				dbItemName(), 0, _mysqlItemType, _flag, pData);
}

bool DBTableItemMysqlPython::writeItemSql(DBContext* pContext, sl::OBStream& data){
	SQLCommand& sqlCommand = pContext->getSqlCommand();
	int32 dataLen = 0;
	const void* pData = data.readBlob(dataLen);
	if(!pData)
		return false;

	sqlCommand.save(Field(dbItemName()).addStruct(pData, dataLen));
	return true;
}

bool DBTableItemMysqlPython::addStream(DBContext* pContext, sl::IBStream& data){
	IMysqlResult* result = pContext->dbResult();
	SLASSERT(result, "wtf");

	int32 dbRowIdx = pContext->dbRowIndex();
	SLASSERT(dbRowIdx < result->rowCount(), "wtf");

	int32 dataSize = 0;
	const void* pData = result->getDataBlob(dbRowIdx, dbItemName(), dataSize);
	data.addBlob(pData, dataSize);
	return true;
}

bool DBTableItemMysqlPython::makeTestData(sl::IBStream& data){
	if(this->uType() > 0)
		data << this->uType();

	int32 rand = sl::getRandom(0, 200);
	char* test = (char*)alloca(rand);
	for(int32 i = 0; i < rand; i++){
		char num = sl::getRandom(0, 255);
		test[i] = num;
	}
	data.addBlob(test, rand);
	return true;
}

bool DBTableItemMysqlBlob::syncToDB(IDBInterface* pdbi, void* pData){
	return DBTableItemMysql::syncItemToDB(pdbi, _dbItemType.c_str(), _tableName.c_str(),
				dbItemName(), 0, _mysqlItemType, _flag, pData);
}

bool DBTableItemMysqlBlob::writeItemSql(DBContext* pContext, sl::OBStream& data){
	SQLCommand& sqlCommand = pContext->getSqlCommand();
	int32 dataLen = 0;
	const void* pData = data.readBlob(dataLen);
	if(!pData)
		return false;

	sqlCommand.save(Field(dbItemName()).addStruct(pData, dataLen));
	return true;
}

bool DBTableItemMysqlBlob::addStream(DBContext* pContext, sl::IBStream& data){
	IMysqlResult* result = pContext->dbResult();
	SLASSERT(result, "wtf");

	int32 dbRowIdx = pContext->dbRowIndex();
	SLASSERT(dbRowIdx < result->rowCount(), "wtf");

	int32 dataSize = 0;
	const void* pData = result->getDataBlob(dbRowIdx, dbItemName(), dataSize);
	data.addBlob(pData, dataSize);
	return true;
}

bool DBTableItemMysqlBlob::makeTestData(sl::IBStream& data){
	int32 rand = sl::getRandom(0, 200);
	char* test = (char*)alloca(rand);
	for(int32 i = 0; i < rand; i++){
		char num = sl::getRandom(0, 255);
		test[i] = num;
	}
	data.addBlob(test, rand);
	return true;
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

uint64 DBTableMysql::writeTable(IDBInterface* pdbi, uint64 dbid, sl::OBStream& data){
	SQLCommand& sqlCommand = static_cast<DataBaseMysql*>(_database)->createSqlCommand();
	DBContext* pDBContext = NEW DBContext(tableName(), sqlCommand, dbid);

	char fixedTableName[256];
	SafeSprintf(fixedTableName, 256, TABLE_PREFIX "_%s", tableName());
	sqlCommand.table(fixedTableName);

	while(data.getSize() > 0){
		int32 propId = 0;
		data >> propId;

		DBTableItem* pTableItem = findItem(propId);
		if(!pTableItem){
			SLASSERT(false, "cant find item[%d] from table [%s]", propId, tableName());
			return 0;
		}

		if(!pTableItem->writeItemSql(pDBContext, data)){
			return 0;
		}
	}

	if(!writeDataToDB(pdbi, pDBContext, dbid == 0))
		return 0;
	
	dbid = pDBContext->dbid();
	DEL pDBContext;
	return dbid;
}

bool DBTableMysql::queryTable(IDBInterface* pdbi, uint64 dbid, sl::IBStream& data){
	SLASSERT(dbid > 0, "wtf");

	SQLCommand& sqlCommand = pdbi->createSqlCommand();
	DBContext* pDBContext = NEW DBContext(tableName(), sqlCommand, dbid); 

	for(auto item : _tableFixedOrderItems){
		if(!item->readItemSql(pDBContext))
			return false;
	}

	if(!readDataFromDB(pdbi, pDBContext, data))
		return false;

	return true;
}

bool DBTableMysql::writeDataToDB(IDBInterface* pdbi, DBContext* pContext, bool isInsert){
	SQLCommand& sqlCommand = pContext->getSqlCommand();
	if(pContext->dbid() > 0)
		pContext->optType() == DB_OPT_DELETE ? sqlCommand.where(Field(TABLE_ID_CONST_STR) == pContext->dbid()) :
			sqlCommand.save(Field(TABLE_ID_CONST_STR) = pContext->dbid());
	
	if(pContext->dbid() <= 0 && pContext->parentDBId() > 0)
		sqlCommand.save(Field(TABLE_PARENTID_CONST_STR) = pContext->parentDBId());
	
	sqlCommand.submit();
	IMysqlResult* writeResult = pdbi->execSqlSync(sqlCommand.optType(), sqlCommand.toString());

	if(!writeResult || writeResult->errCode() != 0){
		SLASSERT(false, "writeDataToDB failed error:%s", writeResult->errInfo());
		return false;
	}
	
	if(writeResult->optType() == DB_OPT_SAVE && pContext->dbid() <= 0)
		pContext->setDBId(writeResult->insertId());
		
	SLASSERT(pContext->dbid() > 0, "wtf");
		
	//如果是删除操作的话，检查是否有子表需要删除
	if(writeResult->optType() == DB_OPT_DELETE){		
		DBTable* pDBTable = _database->findTable(pContext->tableName());
		SLASSERT(pDBTable, "wtf");

		const std::vector<DBTable*>& subDBTables = pDBTable->subTables();
		for(auto subTable : subDBTables){
			pContext->addSubContext(subTable->tableName(), NULL);
		}
	}
	
	DBContext::SUB_CONTEXTS& subDBContexts = pContext->subContexts();
	for(auto contextItor : subDBContexts){
		if(isInsert){
			for(auto cItor : contextItor.second){
				cItor->setParentDBId(pContext->dbid());
				if(!writeDataToDB(pdbi, cItor, isInsert))
					return false;
			}
		}
		else{
			char fixedSubTableName[256];
			SafeSprintf(fixedSubTableName, 256, TABLE_PREFIX "_%s", contextItor.first.c_str());

			SQLCommand& queryCommand = pdbi->createSqlCommand();
			queryCommand.table(fixedSubTableName).where(Field(TABLE_PARENTID_CONST_STR) == pContext->dbid());
			queryCommand.select(TABLE_ID_CONST_STR).get();

			queryCommand.submit();
			IMysqlResult* queryResult = pdbi->execSqlSync(queryCommand.optType(), queryCommand.toString());
			if(!queryResult || queryResult->errCode() != 0){
				SLASSERT(false, "writeDataToDB failed error:%s", queryResult->errInfo());
				return false;
			}
			DEL &queryCommand;
			
			int32 size = contextItor.second.size();
			int32 dbSize = queryResult->rowCount();

			int32 idx = 0;
			while(idx < dbSize){
				//获取数据库中的dbid
				uint64 subId = queryResult->getDataInt64(idx, TABLE_ID_CONST_STR);
				
				if(idx < size){
					//更新列表项，设置dbid
					contextItor.second[idx]->setDBId(subId);
				}
				else{
					//多余的执行删除操作
					SQLCommand& delCommand = pdbi->createSqlCommand();
					DBContext* subContext = NEW DBContext(contextItor.first.c_str(), delCommand, subId);

					delCommand.table(fixedSubTableName).del();
					contextItor.second.push_back(subContext);
				}
				idx++;
			}

			for(auto cItor : contextItor.second){
				cItor->setParentDBId(pContext->dbid());
				if(!writeDataToDB(pdbi, cItor, isInsert))
					return false;
			}
		}
	}
	return true;
}

bool DBTableMysql::readDataFromDB(IDBInterface* pdbi, DBContext* pContext, sl::IBStream& data){
	SQLCommand& sqlCommand = pContext->getSqlCommand();
	sqlCommand.select(TABLE_ID_CONST_STR);

	if(pContext->dbid() > 0)
		sqlCommand.where(Field(TABLE_ID_CONST_STR) == pContext->dbid());

	if(pContext->parentDBId() > 0)
		sqlCommand.where(Field(TABLE_PARENTID_CONST_STR) == pContext->parentDBId());

	sqlCommand.submit();
	IMysqlResult* readResult = pdbi->execSqlSync(sqlCommand.optType(), sqlCommand.toString());
	if(!readResult || readResult->errCode() != 0){
		if(readResult){
			SLASSERT(false, "readDataFromDB failed, error:%s", readResult->errInfo());
			readResult->release();
		}
		return false;
	}
	pContext->setDBResult(readResult);

	const int32 rowCount = readResult->rowCount();
	DBTable* pDBTable = _database->findTable(pContext->tableName());
	SLASSERT(pDBTable, "wtf");
	const std::vector<DBTableItem*>& tableItems = pDBTable->tableItems(); 

	if(pDBTable->isChild()){
		data << rowCount;
	}
	for(int32 idx = 0; idx < rowCount; idx++){
		const uint64 dbid = readResult->getDataInt64(idx, TABLE_ID_CONST_STR);
		pContext->setDBId(dbid);
		pContext->setDBRowIndex(idx);

		for(auto item : tableItems){
			if(!pDBTable->isChild()){
				SLASSERT(item->uType() > 0, "wtf");
				data << item->uType();
			}
			item->addStream(pContext, data);
		}
	}

	return true;
}

bool DBTableMysql::writeItemSql(DBContext* pContext, sl::OBStream& data){
	for(auto item : _tableFixedOrderItems){
		if(!item->writeItemSql(pContext, data))
			return false;
	}
	return true;
}

bool DBTableMysql::readItemSql(DBContext* pContext){
	for(auto item : _tableFixedOrderItems){
		if(!item->readItemSql(pContext))
			return false;
	}
	return true;
}

bool DBTableMysql::makeTestData(sl::IBStream& data){
	for(auto item : _tableFixedOrderItems){
		if(!item->makeTestData(data))
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

bool DataBaseMysql::makeTest(){
	for(auto table : _tables){
		if(strcmp(table.second->tableName(), "Avatar") != 0)
			continue;

		if(table.second->isChild())
			continue;
		sl::BStream<102400> testData;
		if(!table.second->makeTestData(testData)){
			SLASSERT(false, "wtf");
			return false;
		}
		sl::OBStream out = testData.out();
		table.second->writeTable(_dbInterface, (uint64)21, out);
	}
	return true;
}
