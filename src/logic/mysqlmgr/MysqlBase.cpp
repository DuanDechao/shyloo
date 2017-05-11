#include "MysqlBase.h"
#include "MysqlMgr.h"
#include "SQLBase.h"

MysqlBase::MysqlBase(ISLDBConnection* dbConn, SQLCommand* sqlCommand)
	:_dbConnection(dbConn),
	_sqlCommand(sqlCommand)
{}

MysqlBase::~MysqlBase(){
	if (_sqlCommand)
		DEL _sqlCommand;

	_sqlCommand = nullptr;
	_dbConnection = nullptr;
}

bool MysqlBase::onExecute(sl::api::IKernel* pKernel){
	SLASSERT(_sqlCommand->checkVaild(), "invaild sql command");
	if (_sqlCommand->optType() == DB_OPT_QUERY){
		ISLDBResult* dbResult = _dbConnection->executeWithResult(_sqlCommand->toString());
		if (!dbResult){
			_errCode = _dbConnection->getLastErrno();
			SLASSERT(false, "sql command exec %s failed, error:%s", _sqlCommand->toString(), _dbConnection->getLastError());
			return false;
		}

		while (dbResult->next()){
			int32 fieldNum = (int32)dbResult->fieldNum();
			unordered_map<string, string> rowResult;
			for (int32 i = 0; i < fieldNum; i++){
				const char* field = dbResult->fieldValue(i);
				int32 length = dbResult->fieldLength(i) + 1;
				rowResult.insert(make_pair(dbResult->fieldName(i), string(dbResult->fieldValue(i), dbResult->fieldLength(i) + 1)));
			}
			_result.push_back(std::move(rowResult));
		}
	}
	else{
		if (!_dbConnection->execute(_sqlCommand->toString())){
			_errCode = _dbConnection->getLastErrno();
			SLASSERT(false, "sql command exec %s failed, error:%s", _sqlCommand->toString(), _dbConnection->getLastError());
			return false;
		}
	}

	return true;
}

void MysqlBase::Exec(IMysqlHandler* handler){
	handler->setBase(this);
	_handler = handler;
}

bool MysqlBase::onSuccess(sl::api::IKernel* pKernel){
	return _handler->onSuccess(pKernel, _sqlCommand->optType(), _affectedRow, _result);
}

bool MysqlBase::onFailed(sl::api::IKernel* pKernel, bool nonviolent){
	return _handler->onFailed(pKernel, _sqlCommand->optType(), _errCode);
}

void MysqlBase::onRelease(sl::api::IKernel* pKernel){
	if (_handler){
		_handler->setBase(nullptr);
		_handler->onRelease();
		_handler = nullptr;
	}
	DEL this;
}