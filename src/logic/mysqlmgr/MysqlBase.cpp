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
	_errCode = realExecSql(_sqlCommand, _dbConnection, &_result);
	return _errCode == 0;
}

int32 MysqlBase::realExecSql(SQLCommand* sqlCommand, ISLDBConnection* dbConnection, MysqlResult* mysqlResult){
	SLASSERT(sqlCommand->checkVaild(), "invaild sql command");
	int32 errCode = 0;
	if (sqlCommand->optType() == DB_OPT_QUERY){
		ISLDBResult* dbResult = dbConnection->executeWithResult(sqlCommand->toString());
		if (!dbResult){
			errCode = dbConnection->getLastErrno();
			mysqlResult->setErrCode(errCode);
			const char* errInfo = dbConnection->getLastError();
			mysqlResult->setErrInfo(errInfo);
	//		SLASSERT(false, "sql command exec %s failed, error:%s", sqlCommand->toString(), errInfo);
			return errCode;
		}
		mysqlResult->setColumns(dbResult);

		while (dbResult->next()){
			mysqlResult->setColData(dbResult);
		}
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
		SLASSERT(false, "getTableField from table(%s) failed, error:%s", tableName, errInfo);
		return errCode;
	}
	mysqlResult->setColumns(dbResult);

	while(dbResult->next()){
		mysqlResult->setColData(dbResult);
	}
	return errCode;
}

bool MysqlBase::onSuccess(sl::api::IKernel* pKernel){
	return _callback(pKernel, _errCode, _sqlCommand->optType(), _affectedRow, &_result);
}

bool MysqlBase::onFailed(sl::api::IKernel* pKernel, bool nonviolent){
	return _callback(pKernel, _errCode, _sqlCommand->optType(), _affectedRow, &_result);
}

void MysqlBase::onRelease(sl::api::IKernel* pKernel){
	DEL this;
}
