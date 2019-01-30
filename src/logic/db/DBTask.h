#include "slikernel.h"
#include "DB.h"
class DBQueryTask: public sl::api::IAsyncHandler{
public:
	static DBQueryTask* create(const char* tableName, const uint64 dbid, const QueryTableCallBack& cb){
		return NEW DBQueryTask(tableName, dbid, cb);
	}

	static DBQueryTask* create(const int32 tableId, const uint64 dbid, const QueryTableCallBack& cb){
		return NEW DBQueryTask(tableId, dbid, cb);
	}
	
	virtual bool onExecute(sl::api::IKernel* pKernel);
	virtual bool onSuccess(sl::api::IKernel* pKernel);
	virtual bool onFailed(sl::api::IKernel* pKernel, bool nonviolent);
	virtual void onRelease(sl::api::IKernel* pKernel) {	DEL this; }

	inline void setDataBase(DataBase* dataBase) { _database = dataBase;}
	inline DataBase* dataBase() {return _database;} 
	inline const uint64 dbid() {return _dbid;}
	inline const int32 tableId() {return _tableId;}
	inline const char* tableName() {return _tableName.c_str();}

private:
	DBQueryTask(const char* tableName, const uint64 dbid, const QueryTableCallBack& cb)
		:_tableName(tableName),
		 _tableId(0),
		 _queryCB(cb),
		 _dbid(dbid),
		 _database(NULL)
	{}

	DBQueryTask(const int32 tableId, const uint64 dbid, const QueryTableCallBack& cb)
		:_tableName(""),
		 _tableId(tableId),
		 _queryCB(cb),
		 _dbid(dbid),
		 _database(NULL)
	{}

	virtual ~DBQueryTask(){}

private:
	std::string			_tableName;
	int32				_tableId;
	QueryTableCallBack	_queryCB;
	DataBase*			_database;
	uint64				_dbid;
	sl::BStream<10240>	_queryData;
};

class DBWriteTask: public sl::api::IAsyncHandler{
public:
	static DBWriteTask* create(const char* tableName, const uint64 dbid, sl::OBStream& data, const WriteTableCallBack& cb){
		return NEW DBWriteTask(tableName, dbid, data, cb);
	}

	static DBWriteTask* create(const int32 tableId, const uint64 dbid, sl::OBStream& data, const WriteTableCallBack& cb){
		return NEW DBWriteTask(tableId, dbid, data, cb);
	}
	
	virtual bool onExecute(sl::api::IKernel* pKernel);
	virtual bool onSuccess(sl::api::IKernel* pKernel);
	virtual bool onFailed(sl::api::IKernel* pKernel, bool nonviolent);
	virtual void onRelease(sl::api::IKernel* pKernel) {	DEL this; }

	inline void setDataBase(DataBase* dataBase) { _database = dataBase;}
	inline DataBase* dataBase() {return _database;} 
	inline const uint64 dbid() {return _dbid;}
	inline const int32 tableId() {return _tableId;}
	inline const char* tableName() {return _tableName.c_str();}

private:
	DBWriteTask(const char* tableName, const uint64 dbid, sl::OBStream& data, const WriteTableCallBack& cb)
		:_tableName(tableName),
		 _tableId(0),
		 _writeCB(cb),
		 _dbid(dbid),
		 _database(NULL),
		 _insertId(0)
	{
		_writeData << data;
	}

	DBWriteTask(const int32 tableId, const uint64 dbid, sl::OBStream& data, const WriteTableCallBack& cb)
		:_tableName(""),
		 _tableId(tableId),
		 _writeCB(cb),
		 _dbid(dbid),
		 _database(NULL),
		 _insertId(0)
	{
		_writeData << data;
	}

	virtual ~DBWriteTask(){}

private:
	std::string			_tableName;
	int32				_tableId;
	WriteTableCallBack	_writeCB;
	DataBase*			_database;
	uint64				_dbid;
	uint64				_insertId;
	sl::BStream<10240>	_writeData;
};


