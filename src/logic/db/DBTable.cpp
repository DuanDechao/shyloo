#include "DBTable.h"
#include "IDCCenter.h"
DBTableItem::DBTableItem(const char* itemName, IDataType* dataType, const char* defaultVal)
	:_dataType(dataType),
	_itemName(itemName),
	_defaultVal(defaultVal),
	_tableName(NULL),
	_parentTable(NULL),
	_parentTableItem(NULL)
{}
	
DBTable::DBTable(const char* tableName)
	:_tableName(tableName),
	 _isChild(false),
	 _syncState(SYNC_STATE::ST_INIT)
{}

bool DBTable::initialize(){
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
		pDTItem->initialize();
	}
	return true;
}

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

