#include "MysqlMgr.h"
#include "MysqlBase.h"
#include "SQLBuilder.h"
#include "slxml_reader.h"

ISLDBConnection* MysqlMgr::_escapeConnection = nullptr;

bool MysqlMgr::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;

	sl::XmlReader svrConf;
	if (!svrConf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "cant load core file");
		return false;
	}
	const char* host = svrConf.root()["db"][0].getAttributeString("host");
	const int32 port = svrConf.root()["db"][0].getAttributeInt32("port");
	const char* user = svrConf.root()["db"][0].getAttributeString("user");
	const char* pwd = svrConf.root()["db"][0].getAttributeString("pwd");
	const char* dbName = svrConf.root()["db"][0].getAttributeString("db");
	const char* charset = svrConf.root()["db"][0].getAttributeString("charset");
	
	sl::XmlReader conf;
	if (!conf.loadXml(pKernel->getConfigFile())){
		SLASSERT(false, "can not load config file %s", pKernel->getConfigFile());
		return false;
	}
	int32 threadCount = conf.root()["async"][0].getAttributeInt32("threadNum");
	if (threadCount > 0){
		_dbConnectionPool = newDBConnectionPool(threadCount + 1, host, port, user, pwd, dbName, charset);
		SLASSERT(_dbConnectionPool, "create connectionPool failed");
		
		for (int32 i = 0; i < threadCount; i++){
			ISLDBConnection* dbConn = _dbConnectionPool->allocConnection();
			SLASSERT(dbConn, "create db connection failed");
			_dbConnections.push_back(dbConn);
		}

		_escapeConnection = _dbConnectionPool->allocConnection();
		SLASSERT(_escapeConnection, "create db connection failed");
	}

	return true;
}

bool MysqlMgr::launched(sl::api::IKernel * pKernel){
	//test();
	return true;
}

bool MysqlMgr::destory(sl::api::IKernel * pKernel){
	for (auto& dbConn : _dbConnections){
		if (dbConn)
			dbConn->release();
	}
	_dbConnections.clear();

	if (_escapeConnection)
		_escapeConnection->release();
	_escapeConnection = nullptr;

	DEL this;

	return true;
}

void MysqlMgr::execSql(const int64 id, IMysqlHandler* handler, const SQLCommnandFunc& f){
	SLASSERT(!handler->getBase(), "cannot set base twice");
	SQLCommand* sqlCommand = NEW SQLCommand(NEW SQLBuilder());
	if (f){
		f(_kernel, *sqlCommand);
	}

	if (!sqlCommand->submit()){
		SLASSERT(false, "sqlCommand is inVaild");
		return;
	}

	ISLDBConnection* dbConn = _dbConnections[(uint64)id % _dbConnections.size()];
	MysqlBase* mysqlBase = NEW MysqlBase(dbConn, sqlCommand);
	mysqlBase->Exec(handler);
	
	_kernel->startAsync(id, mysqlBase, "exec mysql command");
}

void MysqlMgr::execSql(const int64 id, IMysqlHandler* handler, const char* sql, const char* table, const int8 optType){
	SLASSERT(!handler->getBase() && sql && strcmp(sql, "") != 0, "wtf");
	SQLCommand* sqlCommand = NEW SQLCommand(NEW SQLBuilder(sql, table, optType));
	if (!sqlCommand->checkVaild() && !sqlCommand->submit()){
		SLASSERT(false, "not commit like update¡¢insert¡¢delete¡¢update");
		return;
	}

	ISLDBConnection* dbConn = _dbConnections[(uint64)id % _dbConnections.size()];
	MysqlBase* mysqlBase = NEW MysqlBase(dbConn, sqlCommand);
	mysqlBase->Exec(handler);
	_kernel->startAsync(id, mysqlBase, "exec mysql command");
}

void MysqlMgr::stopSql(IMysqlHandler* handler){
	SLASSERT(handler->getBase(), "mysql handler is not start");
	_kernel->stopAsync((MysqlBase*)handler->getBase());
}

int32 MysqlMgr::escapeString(char* dest, const int32 destSize, const char* src, const int32 srcSize){
	int32 len = _escapeConnection->escapeString(dest, src, srcSize);
	len = len > destSize ? destSize : len;
	return len;
}

void MysqlMgr::test(){
	TestHandler* handler = NEW TestHandler();
	auto f = [&](sl::api::IKernel* pKernel, SQLCommand& sqlCommand){
		sqlCommand.table("user").insert(Field("id") = 1111122661331, Field("name") = "ddc\0ffc");
	};
	execSql(0, handler, f);
}