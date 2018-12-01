#include "DBTable.h"
#include "IDCCenter.h"
DBTableItem::DBTableItem(const char* itemName, IDataType* dataType)
	:_dataType(dataType),
	_itemName(itemName),
	_defaultVal(defaultVal),
	_tableName(NULL),
	_parentTable(NULL),
	_parentTableItem(NULL),
	
{}
	
DBTable::DBTable(const char* tableName)
	:_tableName(tableName),
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

DataBase::DataBase(IDBInterface* pdbi)
	:_dbInterface(pdbi)
{}

void DataBase::addTable(DBTable* pTable){
	auto itor = _tables.find(pTable->tableName());
	if(itor != _tables.end())
		return;
	_tables[pTable->tableName()] = pTable;
}

DBTable* DataBase::findTable(const char* tableName){
	auto itor = _tables.find(tableName);
	if(itor == _tables.end())
		return NULL;
	return itor->second;
}

