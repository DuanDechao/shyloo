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

void DBTable::addItem(DBTableItem* item){
	_tableItems[item->uType()] = item;
	_tableFixedOrderItems.push_back(item);
}

DBTableItem* DBTable::findItem(int32 utype){
	auto itor = _tableItems.find(utype);
	if(itor != _tableItems.end()){
		return itor->second;
	}
	return NULL;
}

uint64 DBTable::writeTable(IDBInterface* pdbi, uint64 dbid, sl::OBStream& data, IObjectDefModule* defModule){
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

DataBase::DataBase(IDBInterface* pdbi)
	:_dbInterface(pdbi)
{}

void DataBase::addTable(DBTable* pTable){
	auto itor = _tables.find(pTable->tableName());
	if(itor != _tables.end())
		return;
	_tables[pTable->tableName()] = pTable;

	auto idItor = _idToTables.find(pTable->tableId());
	if(idItor != _idToTables.end())
		return;
	_idToTables[pTable->tableId()] = pTable;
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