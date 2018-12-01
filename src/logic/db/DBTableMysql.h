#ifndef __SL_DB_TABLE_MYSQL_H__
#define __SL_DB_TABLE_MYSQL_H__
#include "IObjectDef.h"
#include "DBTable.h"
#include "IDB.h"
class DBTableItemMysqlFixedDict: public DBTableItem{
public:
	DBTableItemMysqlFixedDict(const char* itemName, IDataType* dataType, const char* defaultVal);
	virtual ~DBTableItemMysqlFixedDict(){}
	
	virtual bool initialize();
	virtual bool syncToDB(IDBInterface* pdbi) {return true;}

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

class DBTableMysql: public DBTable{
public:
	DBTableMysql(const char* tableName);
	virtual ~DBTableMysql(){}

	virtual DBTableItem* createItem(const char* itemName, IDataType* dataType, const char* defaultVal);
	virtual bool syncToDB(IDBInterface* pdbi);
};

class DataBaseMysql: public DataBase{
public:
	DataBaseMysql(IDBInterface* dbInferface);
	virtual ~DataBaseMysql(){}

	virtual void initialize();
};

#endif
