#ifndef __SL_DB_TABLE_MYSQL_H__
#define __SL_DB_TABLE_MYSQL_H__
#include "IObjectDef.h"
#include "DBTable.h"
#include "IDB.h"
class DBTableItemMysql: public DBTableItem{
public:
	DBTableItemMysql(){}
	virtual ~DBTableItemMysql(){}

	static bool syncItemToDB(IDBInterface* pdbi, const char* dbDataType, const char* tableName, const char* itemName,
		const int32 dataLength, enum_field_types sqlItemType, uint32 itemFlags, void* pData);

};
class DBTableItemMysqlFixedDict: public DBTableItem{
public:
	DBTableItemMysqlFixedDict(const char* itemName, IDataType* dataType, const char* defaultVal);
	virtual ~DBTableItemMysqlFixedDict(){}
	
	virtual bool initialize();
	virtual bool syncToDB(IDBInterface* pdbi);

protected:
	KEYTYPE_MAP		_keyTypes;			//dict类型item的keytype
};

class DBTableItemMysqlArray: public DBTableItem{
public:
	DBTableItemMysqlArray(const char* itemName, IDataType* dataType, const char* defaultVal);
	virtual ~DBTableItemMysqlArray(){}
	
	virtual bool initialize();
	virtual bool syncToDB(IDBInterface* pdbi) {return true;}
protected:
	DBTable*		_childTable;		//字段对应的子表
};

class DBTableItemMysqlVector3: public DBTableItem{
public:
	DBTableItemMysqlVector3(const char* itemName, IDataType* dataType, const char* defaultVal);
	virtual ~DBTableItemMysqlVector3(){}

	virtual bool initialize();
	virtual bool syncToDB(IDBInterface& pdbi) {return true}; 
};

class DBTableMysql: public DBTable{
public:
	DBTableMysql(const char* tableName);
	virtual ~DBTableMysql(){}

	virtual DBTableItem* createItem(const char* itemName, IDataType* dataType, const char* defaultVal);
	virtual bool syncToDB(IDBInterface* pdbi);
	virtual bool initialize();
};

class DataBaseMysql: public DataBase{
public:
	DataBaseMysql(IDBInterface* dbInferface);
	virtual ~DataBaseMysql(){}

	virtual bool initialize();
	virtual bool syncToDB();
};

#endif
