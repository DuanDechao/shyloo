#include "DB.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "IMysqlMgr.h"
//#include "DBCall.h"
#include "IResMgr.h"
#include "DBTableMysql.h"
#include "IDebugHelper.h"
bool DB::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	_database = NULL;
	return true;
}

bool DB::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_mysql, MysqlMgr);

	const char* host = SLMODULE(ResMgr)->getResValueString("dbmgr/mysql/host");
	const char* userName = SLMODULE(ResMgr)->getResValueString("dbmgr/mysql/username");
	const char* pwd = SLMODULE(ResMgr)->getResValueString("dbmgr/mysql/password");
	const char* dbName = SLMODULE(ResMgr)->getResValueString("dbmgr/mysql/name");
	_dbInterface = SLMODULE(MysqlMgr)->createDBInterface(host, 3306, userName, pwd, dbName, "utf8");
	if(!_dbInterface){
		SLASSERT(false, "wtf");
		return true;
	}
	_database = NEW DataBaseMysql(_dbInterface);
	if(!_database->initialize() || !_database->syncToDB()){
		ERROR_LOG("DB initialize failed!");
		return false;
	}

	//test();
	START_TIMER(_self, 0, 1, 2000);
	return true;
}

bool DB::destory(sl::api::IKernel * pKernel){
	if(_database){
		DEL _database;
	}
	DEL this;
	return true;
}
void DB::onTime(sl::api::IKernel* pKernel, int64 timetick){
	test();
	pKernel->shutdown();
}

void DB::test(){
//	_database->readTest();
//	_kernel->shutdown();
}




