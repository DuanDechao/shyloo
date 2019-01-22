#include "DBTable.h"
DBTableItem::DBTableItem(const char* itemName, IDataType* dataType, const char* dbItemType, const uint32 dataLength, const int32 flag)
	:_dataType(dataType),
	_itemName(itemName),
	_tableName(""),
	_tableId(0),
	_parentTable(NULL),
	_parentTableItem(NULL),
	_dbItemType(dbItemType),
	_databaseLength(dataLength),
	_flag(flag),
	_prop(NULL)
{}
	
DBTable::DBTable(const int64 tableId, const char* tableName)
	:_tableName(tableName),
	 _tableId(tableId),
	 _isChild(false),
	 _sync(false)
{}

DBTable::~DBTable(){
	for(auto* itemItor : _tableFixedOrderItems){
		DEL itemItor;
	}
}

void DBTable::addItem(DBTableItem* item){
	_tableItems[item->uType()] = item;
	_tableFixedOrderItems.push_back(item);
}

void DBTable::addSubTable(DBTable* table){
	_subDBTables.push_back(table);
}

DBTableItem* DBTable::findItem(int32 utype){
	auto itor = _tableItems.find(utype);
	if(itor != _tableItems.end()){
		return itor->second;
	}
	return NULL;
}

uint64 DBTable::writeTable(IDBInterface* pdbi, uint64 dbid, sl::OBStream& data){
	return dbid;
}

DataBase::DataBase(IDBInterface* pdbi)
	:_dbInterface(pdbi)
{}

DataBase::~DataBase(){
	for(auto tableItor : _tables){
		DEL tableItor.second;
	}
}

void DataBase::addTable(DBTable* pTable){
	auto itor = _tables.find(pTable->tableName());
	if(itor != _tables.end())
		return;
	_tables[pTable->tableName()] = pTable;

	auto idItor = _idToTables.find(pTable->tableId());
	if(idItor != _idToTables.end())
		return;
	_idToTables[pTable->tableId()] = pTable;
	pTable->setDataBase(this);
}

DBTable* DataBase::findTable(const char* tableName){
	auto itor = _tables.find(tableName);
	if(itor == _tables.end())
		return NULL;
	return itor->second;
}
DBTable* DataBase::findTable(const int64 id){
	auto itor = _idToTables.find(id);
	if(itor == _idToTables.end())
		return NULL;
	return itor->second;
}
