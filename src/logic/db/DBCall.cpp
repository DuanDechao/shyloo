#include "DBCall.h"

void DBCall::query(const char* tableName, const DBQueryCommandFunc& f){
	if (f){
		auto& func = [&f](sl::api::IKernel* pKernel, SQLCommand& sqlCommand){

		}
	}
	_db->getMysqlMgr()->execSql()
}