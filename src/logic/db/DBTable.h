#ifndef __SL_DB_TABLE_H__
#define __SL_DB_TABLE_H__
#include "IObjectDef.h"
#include "IMysqlMgr.h"
#include "IDCCenter.h"
#include <map>
#include "slbinary_stream.h"
#include "DBContext.h"
//在数据表中的一个字段
#define TABLE_ARRAY_ITEM_VALUES_CONST_STR		"values"
#define TABLE_ARRAY_ITEM_VALUE_CONST_STR		"value"
#define TABLE_PARENTID_CONST_STR				"parentID"
#define TABLE_PREFIX							"tbl"
#define TABLE_ID_CONST_STR						"id"
#define TABLE_ITEM_PREFIX						"sm"
class DBTable;
class DBTableItem{
public:
	typedef std::vector<std::pair<std::string, DBTableItem*>> KEYTYPE_MAP;
	DBTableItem(const char* itemName, IDataType* dataType, const char* dbItemType, const uint32 dataLength, const int32 flag);
	virtual ~DBTableItem(){}

	inline const char* itemName() const {return _itemName.c_str();}
	inline  void setTableName(const char* name) {_tableName = name;}
	inline const char* tableName() const {return _tableName.c_str();}
	inline  void setTableId(const int32 tableId) {_tableId = tableId;}
	inline const int32 tableId() const {return _tableId;}
	inline const int32 uType() const {if(!_prop) return -1; return _prop->getIndex(_tableId);}
	inline void setParentTable(DBTable* table) { _parentTable = table;}
	inline DBTable* parentTable() const {return _parentTable;}
	inline void setParentTableItem(DBTableItem* parentItem) {_parentTableItem = parentItem;}
	inline DBTableItem* parentTableItem() const {return _parentTableItem;}
	inline const uint32 databaseLength() const {return _databaseLength;}
	inline const int32 flag() const {return _flag;}
	inline void setProp(const IProp* prop) {_prop = prop;}
	inline const IProp* prop() const {return _prop;}
	inline IDataType* getDataType() const {return _dataType;}

	virtual bool initialize(const char* defaultVal) = 0;
	virtual bool syncToDB(IDBInterface* pdbi, void* data) = 0;
	virtual bool isSameKey(const char* key) {return _itemName == key;}
	virtual bool writeItemSql(DBContext* pContext, sl::OBStream& data) = 0;
	virtual bool makeTestData(sl::IBStream& data) = 0;

protected:
	std::string		_itemName;			//字段名称
	std::string		_tableName;			//表名称
	int32			_tableId;			//表id
	DBTable*		_parentTable;		//父表
	DBTableItem*	_parentTableItem;	//父表对应的key字段
	IDataType*		_dataType;			//字段类型
	uint32			_databaseLength;	//数据库中字段长度
	const IProp*	_prop;				//字段所属的属性
	std::string		_dbItemType;		//数据库创建时的属性
	int32			_flag;				//其他字段flag
};

class DataBase;
class IObjectDefModule;
class DBTable{
public:
	typedef std::map<int32, DBTableItem*> TABLEITEM_MAP;
	DBTable(const int64 tableId, const char* tableName);
	virtual ~DBTable(){}

	inline const char* tableName() const {return _tableName.c_str();}
	inline const int64 tableId() const {return _tableId;} 
	inline void setIsChild(bool isChild) {_isChild = isChild;}
	inline bool isChild() const {return _isChild;}
	inline void setDataBase(DataBase* db) {_database = db;}
	inline DataBase* dataBase() {return _database;}

	void addItem(DBTableItem* item);
	DBTableItem* findItem(int32 utype);
	void addSubTable(DBTable* table);
	const std::vector<DBTable*>& subTables() const {return _subDBTables;}
	const std::vector<DBTableItem*>& tableItems() const {return _tableFixedOrderItems;}

	virtual bool initialize() = 0;
	virtual DBTableItem* createItem(const char* itemName, IDataType* dataType, const char* defaultVal) = 0;
	virtual bool syncToDB(IDBInterface* pdbi) = 0;
	virtual uint64 writeTable(IDBInterface* pdbi, uint64 dbid, sl::OBStream& stream);
	virtual bool makeTestData(sl::IBStream& data) = 0;

protected:
	std::string					_tableName;      //表名称
	int64						_tableId;		 //表id		
	TABLEITEM_MAP				_tableItems;	 //所有的字段	
	DataBase*					_database;		 //所属数据库
	std::vector<DBTableItem*>	_tableFixedOrderItems;
	bool						_isChild;		//是否是子表
	bool						_sync;			//是否已经同步
	std::vector<DBTable*>		_subDBTables;	//子表集
};

class DataBase{
public:
	DataBase(IDBInterface* dbInferface);
	virtual ~DataBase(){}

	virtual bool initialize() = 0;
	virtual bool syncToDB() = 0;
	virtual bool makeTest() = 0;

	void addTable(DBTable* pTable);
	DBTable* findTable(const char* table);
	DBTable* findTable(const int64 id);

protected:
	IDBInterface*	_dbInterface;
	std::unordered_map<std::string, DBTable*> _tables;
	std::unordered_map<int64, DBTable*> _idToTables;
};


#endif
