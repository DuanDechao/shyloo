#include "MysqlBase.h"
#include "MysqlMgr.h"
#include "SQLBase.h"

MysqlBase::MysqlBase(ISLDBConnection* dbConn, SQLCommand* sqlCommand, const SQLExecCallback& callback)
	:_dbConnection(dbConn),
	_sqlCommand(sqlCommand),
	_callback(callback)
{}

MysqlBase::~MysqlBase(){
	if (_sqlCommand)
		DEL _sqlCommand;

	_sqlCommand = nullptr;
	_dbConnection = nullptr;
}

bool MysqlBase::onExecute(sl::api::IKernel* pKernel){
	int32 errCode = realExecSql(_sqlCommand, _dbConnection, &_result);
	return errCode == 0;
}

int32 MysqlBase::realExecSql(SQLCommand* sqlCommand, ISLDBConnection* dbConnection, MysqlResult* mysqlResult){
	SLASSERT(sqlCommand->checkVaild(), "invaild sql command");
	int32 errCode = 0;
	mysqlResult->setOptType(sqlCommand->optType());
	if (sqlCommand->optType() == DB_OPT_QUERY){
		ISLDBResult* dbResult = dbConnection->executeWithResult(sqlCommand->toString());
		errCode = dbConnection->getLastErrno();
		if (!dbResult || errCode){
			mysqlResult->setErrCode(errCode);
			const char* errInfo = dbConnection->getLastError();
			mysqlResult->setErrInfo(errInfo);
			if(dbResult){
				dbResult->release();
			}
	//		SLASSERT(false, "sql command exec %s failed, error:%s", sqlCommand->toString(), errInfo);
			return errCode;
		}
		mysqlResult->setColumns(dbResult);

		while (dbResult->next()){
			mysqlResult->setColData(dbResult);
		}
		dbResult->release();
	}
	else{
		if (!dbConnection->execute(sqlCommand->toString())){
			errCode = dbConnection->getLastErrno();
			mysqlResult->setErrCode(errCode);
			const char* errInfo = dbConnection->getLastError();
			mysqlResult->setErrInfo(errInfo);
	//		SLASSERT(false, "sql command exec %s failed, error:%s", sqlCommand->toString(), errInfo);
			return errCode;
		}
		mysqlResult->setAffectedRows(dbConnection->getAffectedRows());
		mysqlResult->setInsertId(dbConnection->getInsertId());
	}
	return errCode;
}

int32 MysqlBase::getTableFields(ISLDBConnection* dbConnection, const char* tableName, MysqlResult* mysqlResult){
	ISLDBResult* dbResult = dbConnection->getTableFields(tableName);
	int32 errCode = dbConnection->getLastErrno();
	if(!dbResult || errCode){
		mysqlResult->setErrCode(errCode);
		const char* errInfo = dbConnection->getLastError();
		mysqlResult->setErrInfo(errInfo);
		if(dbResult){
			dbResult->release();
		}
		SLASSERT(false, "getTableField from table(%s) failed, error:%s", tableName, errInfo);
		return errCode;
	}
	mysqlResult->setColumns(dbResult);

	while(dbResult->next()){
		mysqlResult->setColData(dbResult);
	}
	dbResult->release();
	return errCode;
}

bool MysqlBase::onSuccess(sl::api::IKernel* pKernel){
	return _callback(pKernel, &_result);
}

bool MysqlBase::onFailed(sl::api::IKernel* pKernel, bool nonviolent){
	return _callback(pKernel, &_result);
}

void MysqlBase::onRelease(sl::api::IKernel* pKernel){
	DEL this;
}
