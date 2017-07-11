#include "DBCall.h"
#include "DBCreator.h"

DBCall::DBCall(string tableName, int64 threadId)
	:_tableName(tableName),
	 _threadId(threadId)
{}

DBCall::~DBCall(){}

void DBCall::exec(const int8 optType, const char* sql, const DBCallBack& cb){
	_cb = cb;
	DBCreator::getMysqlMgr()->execSql(_threadId, this, sql, "create_table", optType);
}


bool DBCall::onSuccess(sl::api::IKernel* pKernel, const int32 optType, const int32 affectedRow, IMysqlResult* result){
	if (_cb)
		_cb(pKernel, _tableName.c_str(), true, result);
	return true;
}

bool DBCall::onFailed(sl::api::IKernel* pKernel, const int32 optType, const int32 errCode){
	if (_cb){
		_cb(pKernel, _tableName.c_str(), false, nullptr);
	}
	return true;
}

void DBCall::onRelease(){
	DEL this;
}

