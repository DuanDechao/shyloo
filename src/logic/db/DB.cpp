#include "DB.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "IMysqlMgr.h"
#include "DBCall.h"
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
	_database->initialize();

	//test();
	return true;
}

bool DB::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

IDBCall* DB::create(int64 threadId, const int64 id, const char* file, const int32 line, const void* context, const int32 size){
	if (size <= SIZE_32){
		return DBContext<SIZE_32>::create(_dbInterface, threadId, id);
	}
	else if (size <= SIZE_64){
		return DBContext<SIZE_64>::create(_dbInterface, threadId, id);
	}
	else if (size <= SIZE_128){
		return DBContext<SIZE_128>::create(_dbInterface, threadId, id);
	}
	else if (size <= SIZE_256){
		return DBContext<SIZE_256>::create(_dbInterface, threadId, id);
	}
	else if (size <= SIZE_512){
		return DBContext<SIZE_512>::create(_dbInterface, threadId, id);
	}
	else if (size <= SIZE_1024){
		return DBContext<SIZE_1024>::create(_dbInterface, threadId, id);
	}
	else if (size <= SIZE_2048){
		return DBContext<SIZE_2048>::create(_dbInterface, threadId, id);
	}
	else if (size <= SIZE_4096){
		return DBContext<SIZE_4096>::create(_dbInterface, threadId, id);
	}
	else{
		SLASSERT(false, "too large");
	}
	return nullptr;
}

void DB::test(){
	//IDBCall* dbcall = DBContext<SIZE_64>::create(_self, 0, 0);
}




