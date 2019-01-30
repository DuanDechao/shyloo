#include "MysqlMgr.h"
#include "DBInterface.h"
bool MysqlMgr::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	return true;
}

bool MysqlMgr::launched(sl::api::IKernel * pKernel){
	//test();
	return true;
}

bool MysqlMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}
	
IDBInterface* MysqlMgr::createDBInterface(const char* host, const int32 port, const char* user, const char* pwd, const char* dbName, const char* charset){
	return DBInterface::create(host, port, user, pwd, dbName, charset);
}
