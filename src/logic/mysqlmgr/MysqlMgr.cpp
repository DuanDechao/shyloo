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
	return true;
}
	
IDBInterface* MysqlMgr::createDBInterface(const char* host, const int32 port, const char* user, const char* pwd, const char* dbName, const char* charset, int32 threadNum){
	std::stringstream dbIdx;
	dbIdx << host << ":" << port << ":" << dbName;
	auto dbItor = _dbInterfaces.find(dbIdx.str());
	if(dbItor != _dbInterfaces.end()){
		return dbItor->second;
	}
	DBInterface* dbInterface = DBInterface::create(host, port, user, pwd, dbName, charset, threadNum);
	_dbInterfaces[dbIdx.str()] = dbInterface;
	return dbInterface;
}

void MysqlMgr::closeDBInterface(const char* host, const int32 port, const char* dbName){
	std::stringstream dbIdx;
	dbIdx << host << ":" << port << ":" << dbName;
	auto dbItor = _dbInterfaces.find(dbIdx.str());
	if(dbItor == _dbInterfaces.end())
		return;
	
	dbItor->second->release();
	_dbInterfaces.erase(dbItor);
}
