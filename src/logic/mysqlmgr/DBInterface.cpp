#include "DBInterface.h"
#include "MysqlBase.h"
#include "SQLBuilder.h"
#include "slxml_reader.h"
#include "MysqlMgr.h"
DBInterface::DBInterface(const char* host, const int32 port, const char* user, const char* pwd, const char* dbName, const char* charset, int32 threadCount)
	:_host(host),
	_port(port),
	_user(user),
	_pwd(pwd),
	_dbName(dbName),
	_charset(charset),
	_threadCount(threadCount),
	_dbConnectionPool(NULL),
	_escapeConnection(NULL)
{
	threadCount = threadCount <= 0 ? 1: threadCount;
	_dbConnectionPool = newDBConnectionPool(threadCount + 1, host, port, user, pwd, dbName, charset);
	SLASSERT(_dbConnectionPool, "create connectionPool failed");
	
	for (int32 i = 0; i < threadCount; i++){
		ISLDBConnection* dbConn = _dbConnectionPool->allocConnection();
		SLASSERT(dbConn, "create db connection failed");
		_dbConnections.push_back(dbConn);
	}

	_escapeConnection = _dbConnectionPool->allocConnection();
	SLASSERT(_escapeConnection, "create db connection failed");
	_syncConnection = _dbConnectionPool->allocConnection();
	SLASSERT(_syncConnection, "create db connection failed");
}

DBInterface::~DBInterface(){
	for (auto& dbConn : _dbConnections){
		if (dbConn)
			dbConn->release();
	}
	_dbConnections.clear();

	if (_escapeConnection)
		_escapeConnection->release();
	_escapeConnection = nullptr;
	
	if (_syncConnection)
		_syncConnection->release();
	_syncConnection = nullptr;
}

void DBInterface::execSql(const int64 id, const SQLCommnandFunc& f, const SQLExecCallback& cb){
	SQLCommand* sqlCommand = NEW SQLCommand(NEW SQLBuilder(this));
	if (!f || !f(MysqlMgr::getInstance()->getKernel(), *sqlCommand)){
		DEL sqlCommand;
		return;
	}

	if (!sqlCommand->submit()){
		SLASSERT(false, "sqlCommand is inVaild");
		DEL sqlCommand;
		return;
	}

	ISLDBConnection* dbConn = _dbConnections[(uint64)id % _dbConnections.size()];
	MysqlBase* mysqlBase = NEW MysqlBase(dbConn, sqlCommand, cb);
	
	MysqlMgr::getInstance()->getKernel()->startAsync(id, mysqlBase, "exec mysql command");
}

void DBInterface::execSql(const int64 id, const int32 optType, const char* sql, const SQLExecCallback& cb){
	SLASSERT(sql && strcmp(sql, "") != 0, "wtf");
	SQLCommand* sqlCommand = NEW SQLCommand(NEW SQLBuilder(this, optType, sql));
	if (!sqlCommand->submit()){
		SLASSERT(false, "not commit like update、insert、delete、update");
		DEL sqlCommand;
		return;
	}

	ISLDBConnection* dbConn = _dbConnections[(uint64)id % _dbConnections.size()];
	MysqlBase* mysqlBase = NEW MysqlBase(dbConn, sqlCommand, cb);

	MysqlMgr::getInstance()->getKernel()->startAsync(id, mysqlBase, "exec mysql command");
}

IMysqlResult* DBInterface::execSqlSync(const SQLCommnandFunc& f){
	SQLCommand* sqlCommand = NEW SQLCommand(NEW SQLBuilder(this));
	if (!f || !f(MysqlMgr::getInstance()->getKernel(), *sqlCommand)){
		DEL sqlCommand;
		return NULL;
	}
	
	if (!sqlCommand->submit()){
		SLASSERT(false, "sqlCommand is inVaild");
		DEL sqlCommand;
		return NULL;
	}
	
	MysqlResult* result = NEW MysqlResult();
	MysqlBase::realExecSql(sqlCommand, _syncConnection, result);
	return result;
}

IMysqlResult* DBInterface::execSqlSync(const int32 optType, const char* sql){
	SQLCommand* sqlCommand = NEW SQLCommand(NEW SQLBuilder(this, optType, sql));
	if (!sqlCommand->submit()){
		SLASSERT(false, "not commit like update、insert、delete、update");
		DEL sqlCommand;
		return NULL;
	}

	MysqlResult* result = NEW MysqlResult();
	MysqlBase::realExecSql(sqlCommand, _syncConnection, result);
	DEL sqlCommand;
	return result;
}

int32 DBInterface::escapeString(char* dest, const int32 destSize, const char* src, const int32 srcSize){
	int32 len = _escapeConnection->escapeString(dest, src, srcSize);
	len = len > destSize ? destSize : len;
	return len;
}

IMysqlResult* DBInterface::getTableFields(const char* tableName){
	MysqlResult* result = NEW MysqlResult();
	MysqlBase::getTableFields(_syncConnection, tableName, result);
	return result;
}

SQLCommand& DBInterface::createSqlCommand(){
	SQLCommand* sqlCommand = NEW SQLCommand(NEW SQLBuilder(this));
	return *sqlCommand;
}

void DBInterface::test(){
	/*auto f = [&](sl::api::IKernel* pKernel, SQLCommand& sqlCommand){
		sqlCommand.table("user").insert(Field("id") = 1111122661331, Field("name") = "ddc\0ffc");
	};
	execSql(0, handler, f);
	*/
}
