#include "MysqlMgr.h"
#include "MysqlBase.h"
#include "SQLBuilder.h"
bool MysqlMgr::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	return true;
}

bool MysqlMgr::launched(sl::api::IKernel * pKernel){
	test();
	return true;
}

bool MysqlMgr::destory(sl::api::IKernel * pKernel){
	for (auto& dbConn : _dbConnections){
		if (dbConn)
			dbConn->release();
	}
	_dbConnections.clear();

	DEL this;

	return true;
}

void MysqlMgr::execSql(const int64 id, IMysqlHandler* handler, SQLCommnandFunc& f){
	SLASSERT(!handler->getBase(), "cannot set base twice");
	SQLCommand* sqlCommand = NEW SQLCommand(NEW SQLBuilder());
	if (f){
		f(_kernel, *sqlCommand);
	}

	if (!sqlCommand->checkVaild() || !sqlCommand->submit()){
		SLASSERT(false, "sqlCommand is inVaild");
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
	sl::SafeMemcpy(dest, destSize, src, srcSize);
	return srcSize;
}

void MysqlMgr::test(){
	SQLCommand sql(NEW SQLBuilder());
	sql.table("actor").select("dv", "dg").where(Field("id") == 12341, Field("name") == "ddc").get();
	sql.submit();
	const char* sqlcommand  = sql.toString();
	ECHO_TRACE("sql Command:%s", sqlcommand);
	int32 i = 0;
	i++;
}