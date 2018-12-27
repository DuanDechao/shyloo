#include "DB.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "IMysqlMgr.h"
//#include "DBCall.h"
#include "IResMgr.h"
#include "DBTableMysql.h"
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
		SLASSERT(false, "wtf");
		return false;
	}

	test();
	return true;
}

bool DB::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void DB::test(){
	_database->makeTest();
}




