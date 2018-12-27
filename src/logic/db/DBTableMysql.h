#ifndef __SL_DB_TABLE_MYSQL_H__
#define __SL_DB_TABLE_MYSQL_H__
#include "IObjectDef.h"
#include "DBTable.h"
#include "IDB.h"
#include "mysql.h"
class DBTableItemMysql: public DBTableItem{
public:
	DBTableItemMysql(const char* itemName, IDataType* dataType, const char* dbItemType, 
			const uint32 dataLength, const int32 flag, enum_field_types mysqlItemType)
		:DBTableItem(itemName, dataType, dbItemType, dataLength, flag),
		 _mysqlItemType(mysqlItemType)
	{
		s_allMysqlFlags |= flag;
		sl::SafeMemset(_dbItemName, 256, 0, 256);
	}
	virtual ~DBTableItemMysql(){}

	virtual bool initialize(const char* defaultVal) {return true;}
	virtual bool syncToDB(IDBInterface* pdbi, void* pData) = 0;
	virtual void initDBItemName(const char* extraFlag = "");
	virtual bool isSameKey(const char* key) {return strcmp(key, dbItemName()) == 0;}
	virtual bool writeItemSql(DBContext* pContext, sl::OBStream& data) {return true;}
	virtual bool readItemSql(DBContext* pContext) {return true;}
	virtual bool makeTestData(sl::IBStream& data) {return true;}
	
	const char* dbItemName() const {return _dbItemName;}
	static bool syncItemToDB(IDBInterface* pdbi, const char* dbDataType, const char* tableName, const char* itemName,
		const int32 dataLength, enum_field_types sqlItemType, uint32 itemFlags, void* pData);

protected:
	enum_field_types _mysqlItemType;
	static uint32	 s_allMysqlFlags;
	char			 _dbItemName[256];
};

class DBTableItemMysqlDigit: public DBTableItemMysql{
public:
	DBTableItemMysqlDigit(const char* itemName, IDataType* dataType, const char* dbItemType, 
			const uint32 dataLength, const int32 flag, enum_field_types mysqlItemType)
		:DBTableItemMysql(itemName, dataType, dbItemType, dataLength, flag, mysqlItemType)
	{}
	virtual ~DBTableItemMysqlDigit(){}
	
	virtual bool syncToDB(IDBInterface* pdbi, void* data);
	virtual bool writeItemSql(DBContext* pContext, sl::OBStream& data);
	virtual bool readItemSql(DBContext* pContext);

	virtual bool makeTestData(sl::IBStream& data);
};

class DBTableItemMysqlFixedDict: public DBTableItemMysql{
public:
	DBTableItemMysqlFixedDict(const char* itemName, IDataType* dataType, const char* dbItemType, 
			const uint32 dataLength, const int32 flag, enum_field_types mysqlItemType)
		:DBTableItemMysql(itemName, dataType, dbItemType, dataLength, flag, mysqlItemType)
	{}
	virtual ~DBTableItemMysqlFixedDict(){}
	
	virtual bool initialize(const char* defaultVal);
	virtual bool syncToDB(IDBInterface* pdbi, void* pData);
	virtual void initDBItemName(const char* extraFlag);
	virtual bool isSameKey(const char* key);
	virtual bool writeItemSql(DBContext* pContext, sl::OBStream& data);
	virtual bool readItemSql(DBContext* pContext);
	virtual bool makeTestData(sl::IBStream& data);

	const KEYTYPE_MAP& keyTypes() const {return _keyTypes;}

protected:
	KEYTYPE_MAP		_keyTypes;			//dict类型item的keytype
};

class DBTableItemMysqlArray: public DBTableItemMysql{
public:
	DBTableItemMysqlArray(const char* itemName, IDataType* dataType, const char* dbItemType, 
			const uint32 dataLength, const int32 flag, enum_field_types mysqlItemType)
		:DBTableItemMysql(itemName, dataType, dbItemType, dataLength, flag, mysqlItemType),
		_childTable(NULL)
	{}
	virtual ~DBTableItemMysqlArray(){}
	
	virtual bool initialize(const char* defaultVal);
	virtual bool syncToDB(IDBInterface* pdbi, void* pData) {return true;}
	virtual void initDBItemName(const char* extraFlag);
	virtual bool isSameKey(const char* key) {return false;}
	virtual bool writeItemSql(DBContext* pContext, sl::OBStream& data);
	virtual bool readItemSql(DBContext* pContext);
	virtual bool makeTestData(sl::IBStream& data);

protected:
	DBTable*		_childTable;		//字段对应的子表
};

class DBTableItemMysqlVector3: public DBTableItemMysql{
public:
	DBTableItemMysqlVector3(const char* itemName, IDataType* dataType, const char* dbItemType, 
			const uint32 dataLength, const int32 flag, enum_field_types mysqlItemType)
		:DBTableItemMysql(itemName, dataType, dbItemType, dataLength, flag, mysqlItemType)
	{}
	virtual ~DBTableItemMysqlVector3(){}
	
	virtual void initDBItemName(const char* extraFlag);
	virtual bool syncToDB(IDBInterface* pdbi, void* pData);
	virtual bool isSameKey(const char* key);
	virtual bool writeItemSql(DBContext* pContext, sl::OBStream& data);
	virtual bool readItemSql(DBContext* pContext);
	virtual bool makeTestData(sl::IBStream& data);
	
	char** dbItemNames() {return _dbItemNames;}

protected:
	char  _dbItemNames[3][256];
};

class DBTableItemMysqlVector2: public DBTableItemMysql{
public:
	DBTableItemMysqlVector2(const char* itemName, IDataType* dataType, const char* dbItemType,
			const uint32 dataLength, const int32 flag, enum_field_types mysqlItemType)
		:DBTableItemMysql(itemName, dataType, dbItemType, dataLength, flag, mysqlItemType)
	{}
	virtual ~DBTableItemMysqlVector2(){}
	
	virtual void initDBItemName(const char* extraFlag);
	virtual bool syncToDB(IDBInterface* pdbi, void* pData);
	virtual bool isSameKey(const char* key);
	virtual bool writeItemSql(DBContext* pContext, sl::OBStream& data);
	virtual bool readItemSql(DBContext* pContext);
	virtual bool makeTestData(sl::IBStream& data);

	char** dbItemNames() {return _dbItemNames;}

protected:
	char _dbItemNames[2][256];
};

class DBTableItemMysqlVector4: public DBTableItemMysql{
public:
	DBTableItemMysqlVector4(const char* itemName, IDataType* dataType, const char* dbItemType,
			const uint32 dataLength, const int32 flag, enum_field_types mysqlItemType)
		:DBTableItemMysql(itemName, dataType, dbItemType, dataLength, flag, mysqlItemType)
	{}
	virtual ~DBTableItemMysqlVector4(){}
	
	virtual void initDBItemName(const char* extraFlag);
	virtual bool syncToDB(IDBInterface* pdbi, void* pData);
	virtual bool isSameKey(const char* key);
	virtual bool writeItemSql(DBContext* pContext, sl::OBStream& data);
	virtual bool readItemSql(DBContext* pContext);
	virtual bool makeTestData(sl::IBStream& data);

	char** dbItemNames() {return _dbItemNames;}

protected:
	char _dbItemNames[4][256];
};

class DBTableItemMysqlString: public DBTableItemMysql{
public:
	DBTableItemMysqlString(const char* itemName, IDataType* dataType, const char* dbItemType,
			const uint32 dataLength, const int32 flag, enum_field_types mysqlItemType)
		:DBTableItemMysql(itemName, dataType, dbItemType, dataLength, flag, mysqlItemType)
	{}
	virtual ~DBTableItemMysqlString(){}

	virtual bool syncToDB(IDBInterface* pdbi, void* pData);
	virtual bool writeItemSql(DBContext* pContext, sl::OBStream& data);
	virtual bool readItemSql(DBContext* pContext);
	virtual bool makeTestData(sl::IBStream& data);
};

class DBTableItemMysqlUnicode: public DBTableItemMysql{
public:
	DBTableItemMysqlUnicode(const char* itemName, IDataType* dataType, const char* dbItemType,
			const uint32 dataLength, const int32 flag, enum_field_types mysqlItemType)
		:DBTableItemMysql(itemName, dataType, dbItemType, dataLength, flag, mysqlItemType)
	{}
	virtual ~DBTableItemMysqlUnicode(){}

	virtual bool syncToDB(IDBInterface* pdbi, void* pData);
	virtual bool writeItemSql(DBContext* pContext, sl::OBStream& data);
	virtual bool makeTestData(sl::IBStream& data);
};

class DBTableItemMysqlPython: public DBTableItemMysql{
public:
	DBTableItemMysqlPython(const char* itemName, IDataType* dataType, const char* dbItemType,
			const uint32 dataLength, const int32 flag, enum_field_types mysqlItemType)
		:DBTableItemMysql(itemName, dataType, dbItemType, dataLength, flag, mysqlItemType)
	{}
	virtual ~DBTableItemMysqlPython(){}
	
	virtual bool syncToDB(IDBInterface* pdbi, void* pData);
	virtual bool writeItemSql(DBContext* pContext, sl::OBStream& data);
	virtual bool makeTestData(sl::IBStream& data);
};

class DBTableItemMysqlBlob: public DBTableItemMysql{
public:
	DBTableItemMysqlBlob(const char* itemName, IDataType* dataType, const char* dbItemType,
			const uint32 dataLength, const int32 flag, enum_field_types mysqlItemType)
		:DBTableItemMysql(itemName, dataType, dbItemType, dataLength, flag, mysqlItemType)
	{}
	virtual ~DBTableItemMysqlBlob(){}

	virtual bool syncToDB(IDBInterface* pdbi, void* pData);
	virtual bool writeItemSql(DBContext* pContext, sl::OBStream& data);
	virtual bool makeTestData(sl::IBStream& data);
};

class DBTableItemMysqlMailBox: public DBTableItemMysql{
public:
	DBTableItemMysqlMailBox(const char* itemName, IDataType* dataType, const char* dbItemType,
			const uint32 dataLength, const int32 flag, enum_field_types mysqlItemType)
		:DBTableItemMysql(itemName, dataType, dbItemType, dataLength, flag, mysqlItemType)
	{}
	virtual ~DBTableItemMysqlMailBox(){}
	
	virtual bool syncToDB(IDBInterface* pdbi, void* pData) {return true;}
};

class DBTableMysql: public DBTable{
public:
	DBTableMysql(const int64 tableId, const char* tableName);
	virtual ~DBTableMysql(){}

	virtual DBTableItem* createItem(const char* itemName, IDataType* dataType, const char* defaultVal);
	virtual bool syncToDB(IDBInterface* pdbi);
	virtual bool initialize();
	virtual uint64 writeTable(IDBInterface* pdbi, uint64 dbid, sl::OBStream& stream);
	virtual bool queryTable(IDBInterface* pdbi, uint64 dbid, sl::IBStream& stream); 
	virtual bool writeItemSql(DBContext* pContext, sl::OBStream& data);
	virtual bool makeTestData(sl::IBStream& data);

	bool writeDataToDB(IDBInterface* pdbi, DBContext* pContext, bool isInsert);
	bool readDataFromDB(IDBInterface* pdbi, DBContext* pContext, sl::IBStream& data);
	void initDBItemName();

protected:
	bool syncIndexToDB(IDBInterface* pdbi);
};

class DataBaseMysql: public DataBase{
public:
	DataBaseMysql(IDBInterface* dbInferface);
	virtual ~DataBaseMysql(){}

	virtual bool initialize();
	virtual bool syncToDB();
	virtual bool makeTest();
	
	SQLCommand& createSqlCommand() {return _dbInterface->createSqlCommand();}
};

#endif
