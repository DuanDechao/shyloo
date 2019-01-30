#ifndef __SL_CORE_DB_CONTEXT_H__
#define __SL_CORE_DB_CONTEXT_H__
#include <unordered_map>
class DBContext{
public:
	typedef std::unordered_map<std::string, std::vector<DBContext*>> SUB_CONTEXTS;
	DBContext(IDBInterface* pdbi, const char* tableName, const uint64 dbid)
		:_pdbi(pdbi),
		_dbid(dbid),
		_parentDBId(0),
		_tableName(tableName),
		_dbResult(NULL),
		_threadId(0)
	{
		_sqlCommand = pdbi->createSqlCommand(dbid);
	}

	virtual ~DBContext(){
		if(_sqlCommand){
			_sqlCommand->release();
		}
		_sqlCommand = NULL;

		for(auto itor : _subDBContexts){
			for(auto context : itor.second){
				DEL context;
			}
			itor.second.clear();
		}
		_subDBContexts.clear();

		if(_dbResult){
			_dbResult->release();
			_dbResult = NULL;
		}
	}

	void addSubContext(std::string tableName, DBContext* context){
		if(context == NULL){
			std::vector<DBContext*> empty;
			_subDBContexts[tableName] = empty; 
		}
		else{
			context->setThreadId(threadId());
			_subDBContexts[tableName].push_back(context);	
		}
	}

	SUB_CONTEXTS& subContexts() {return _subDBContexts;}

	inline uint64 dbid() const {return _dbid;}
	inline void setDBId(const uint64 dbid) {_dbid = dbid;}
	inline IDBInterface* getDBInterface() {return _pdbi;}
	inline SQLCommand* getSqlCommand() {return _sqlCommand;}
	inline const int32 optType() const {return _sqlCommand->optType();}
	inline const char* tableName() const {return _tableName.c_str();}
	inline void setParentDBId(const uint64 dbid) {_parentDBId = dbid;}
	inline uint64 parentDBId() const {return _parentDBId;}
	inline IMysqlResult* dbResult() const {return _dbResult;}
	inline void setDBResult(IMysqlResult* result) {_dbResult = result;}
	inline int32 dbRowIndex() const {return _dbRowIdx;}
	inline void setDBRowIndex(const int32 idx) {_dbRowIdx = idx;} 
	inline void setThreadId(const int64 threadId) {_threadId = threadId;}
	inline const uint64 threadId() const {return _threadId;}

protected:
	uint64			_dbid;
	uint64			_parentDBId;
	IDBInterface*	_pdbi;
	SQLCommand*		_sqlCommand;
	SUB_CONTEXTS	_subDBContexts;
	std::string		_tableName;
	IMysqlResult*	_dbResult;
	int32			_dbRowIdx;
	uint64			_threadId;
};
#endif
