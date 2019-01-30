#include "DB.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "IMysqlMgr.h"
//#include "DBCall.h"
//#include "IResMgr.h"
#include "DBTableMysql.h"
#include "IDebugHelper.h"
#include "DBTask.h"
#include "slbinary_stream.h"
bool DB::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	_database = NULL;
	return true;
}

bool DB::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_mysql, MysqlMgr);

	const char* host = pKernel->getResValueString("dbmgr/mysql/host");
	const char* userName = pKernel->getResValueString("dbmgr/mysql/username");
	const char* pwd = pKernel->getResValueString("dbmgr/mysql/password");
	const char* dbName = pKernel->getResValueString("dbmgr/mysql/name");
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

bool DB::queryTable(const char* tableName, const uint64 id, const QueryTableCallBack& cb){
	DBQueryTask* task = DBQueryTask::create(tableName, id, cb);
	task->setDataBase(_database);
	_kernel->startAsync(id, task, "DB::queryTable");	
	return true;
}

bool DB::queryTable(const int32 tableId, const uint64 id, const QueryTableCallBack& cb){
	DBQueryTask* task = DBQueryTask::create(tableId, id, cb);
	task->setDataBase(_database);
	_kernel->startAsync(id, task, "DB::queryTable");
	return true;
}

bool DB::writeTable(const char* tableName, const uint64 id, sl::OBStream& data, const WriteTableCallBack& cb){
	DBWriteTask* task = DBWriteTask::create(tableName, id, data, cb);
	task->setDataBase(_database);
	_kernel->startAsync(id, task, "DB::writeTable");
	return true;
}

bool DB::writeTable(const int32 tableId, const uint64 id, sl::OBStream& data, const WriteTableCallBack& cb){
	DBWriteTask* task = DBWriteTask::create(tableId, id, data, cb);
	task->setDataBase(_database);
	_kernel->startAsync(id, task, "DB::writeTable");
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
	auto queryCB = [&](sl::api::IKernel* pKernel, bool success, const char* tableName, const int32 tableId, sl::OBStream& data){
		if(success){
			if(data.getSize() <= 0){
				ERROR_LOG("query Table avatar data empty");
				return;
			}
			auto writeCB = [&](sl::api::IKernel* pKernel, bool success, const char* tableName, const int32 tableId, const uint64 insertId){
				DEBUG_LOG("write table Avatar insert id :%lld", insertId);
			};
			writeTable("Avatar", (const uint64)3, data, writeCB);
		}
		else{
			ERROR_LOG("query table Avatar failed");
		}
	};
	queryTable("Avatar", (const uint64)3, queryCB);
}

void DB::test2(){
	sl::BStream<10240> testData;
	if(!_database->getTestData("Avatar", testData)){
		SLASSERT(false, "wtf");
		return;
	}
	auto writeCB = [&](sl::api::IKernel* pKernel, bool success, const char* tableName, const int32 tableId, const uint64 insertId){
		TRACE_LOG("test2 writeDB insertId %lld", insertId);
	};
	sl::OBStream outData = testData.out();
	writeTable("Avatar", (const int64)3, outData, writeCB);
}




