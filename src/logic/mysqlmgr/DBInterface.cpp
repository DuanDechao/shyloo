#include "DBInterface.h"
#include "MysqlBase.h"
#include "SQLBuilder.h"
#include "slxml_reader.h"
#include "MysqlMgr.h"
DBInterface::DBInterface(const char* host, const int32 port, const char* user, const char* pwd, const char* dbName, const char* charset)
	:_host(host),
	_port(port),
	_user(user),
	_pwd(pwd),
	_dbName(dbName),
	_charset(charset),
	_connectNum(1),
	_dbConnectionPool(NULL)
{
	_connectNum = MysqlMgr::getInstance()->getKernel()->maxAsyncThreadNum();
	_connectNum = _connectNum <= 0 ? 1 : _connectNum;
	_dbConnectionPool = newDBConnectionPool(_connectNum, host, port, user, pwd, dbName, charset);
	SLASSERT(_dbConnectionPool, "create connectionPool failed");
	
	for (int32 i = 0; i < _connectNum; i++){
		ISLDBConnection* dbConn = _dbConnectionPool->allocConnection();
		SLASSERT(dbConn, "create db connection failed");
		_dbConnections.push_back(dbConn);
	}
}

DBInterface::~DBInterface(){
	for (auto& dbConn : _dbConnections){
		if (dbConn){
			dbConn->release();
		}
	}
	_dbConnections.clear();

	if(_dbConnectionPool){
		_dbConnectionPool->release();
	}
	_dbConnectionPool = NULL;
}

bool DBInterface::execSql(const int64 id, const SQLCommnandFunc& f, const SQLExecCallback& cb){
	ISLDBConnection* dbConn = _dbConnections[(uint64)id % _dbConnections.size()];
	SQLCommand* sqlCommand = SQLCommand::create(SQLBuilder::create(dbConn));
	if (!f || !f(MysqlMgr::getInstance()->getKernel(), sqlCommand)){
		sqlCommand->release();
		return false;
	}

	if (!sqlCommand->submit()){
		SLASSERT(false, "sqlCommand is inVaild");
		sqlCommand->release();
		return false;
	}

	MysqlBase* mysqlBase = NEW MysqlBase(dbConn, sqlCommand, cb);
	MysqlMgr::getInstance()->getKernel()->startAsync(id, mysqlBase, "exec mysql command");
	return true;
}

bool DBInterface::execSql(const int64 id, const int32 optType, const char* sql, const SQLExecCallback& cb){
	SLASSERT(sql && strcmp(sql, "") != 0, "wtf");
	ISLDBConnection* dbConn = _dbConnections[(uint64)id % _dbConnections.size()];
	SQLCommand* sqlCommand = SQLCommand::create(SQLBuilder::create(dbConn, optType, sql));
	if (!sqlCommand->submit()){
		SLASSERT(false, "not commit like update、insert、delete、update");
		sqlCommand->release();
		return false;
	}

	MysqlBase* mysqlBase = NEW MysqlBase(dbConn, sqlCommand, cb);
	MysqlMgr::getInstance()->getKernel()->startAsync(id, mysqlBase, "exec mysql command");
	return true;
}

bool DBInterface::execSql(const int64 id, SQLCommand* sqlCommand, const SQLExecCallback& cb){
	if (!sqlCommand->submit()){
		SLASSERT(false, "sqlCommand is inVaild");
		return false;
	}
	
	ISLDBConnection* dbConn = _dbConnections[(uint64)id % _dbConnections.size()];
	MysqlBase* mysqlBase = NEW MysqlBase(dbConn, sqlCommand, cb);
	MysqlMgr::getInstance()->getKernel()->startAsync(id, mysqlBase, "exec mysql command");
	return true;
}

IMysqlResult* DBInterface::execSqlSync(const int64 id, const SQLCommnandFunc& f){
	ISLDBConnection* syncConnection = _dbConnections[(uint64)id % _dbConnections.size()];
	SQLCommand* sqlCommand = SQLCommand::create(SQLBuilder::create(syncConnection));
	if (!f || !f(MysqlMgr::getInstance()->getKernel(), sqlCommand)){
		sqlCommand->release();
		return NULL;
	}
	
	if (!sqlCommand->submit()){
		SLASSERT(false, "sqlCommand is inVaild");
		sqlCommand->release();
		return NULL;
	}
	
	MysqlResult* result = NEW MysqlResult();
	MysqlBase::realExecSql(sqlCommand, syncConnection, result);
	sqlCommand->release();
	return result;
}

IMysqlResult* DBInterface::execSqlSync(const int64 id, const int32 optType, const char* sql){
	ISLDBConnection* syncConnection = _dbConnections[(uint64)id % _dbConnections.size()];
	SQLCommand* sqlCommand = SQLCommand::create(SQLBuilder::create(syncConnection, optType, sql));
	if (!sqlCommand->submit()){
		SLASSERT(false, "not commit like update、insert、delete、update");
		sqlCommand->release();
		return NULL;
	}

	MysqlResult* result = NEW MysqlResult();
	MysqlBase::realExecSql(sqlCommand, syncConnection, result);
	sqlCommand->release();
	return result;
}

IMysqlResult* DBInterface::execSqlSync(const int64 id, SQLCommand* sqlCommand){
	if (!sqlCommand->submit()){
		SLASSERT(false, "sqlCommand is inVaild");
		return NULL;
	}
	
	ISLDBConnection* syncConnection = _dbConnections[(uint64)id % _dbConnections.size()];
	MysqlResult* result = NEW MysqlResult();
	MysqlBase::realExecSql(sqlCommand, syncConnection, result);
	return result;
}

IMysqlResult* DBInterface::getTableFields(const int64 id, const char* tableName){
	ISLDBConnection* syncConnection = _dbConnections[(uint64)id % _dbConnections.size()];
	MysqlResult* result = NEW MysqlResult();
	MysqlBase::getTableFields(syncConnection, tableName, result);
	return result;
}

SQLCommand* DBInterface::createSqlCommand(const int64 id){
	ISLDBConnection* syncConnection = _dbConnections[(uint64)id % _dbConnections.size()];
	SQLCommand* sqlCommand = SQLCommand::create(SQLBuilder::create(syncConnection));
	return sqlCommand;
}

void DBInterface::test(){
	/*auto f = [&](sl::api::IKernel* pKernel, SQLCommand& sqlCommand){
		sqlCommand.table("user").insert(Field("id") = 1111122661331, Field("name") = "ddc\0ffc");
	};
	execSql(0, handler, f);
	*/
}
