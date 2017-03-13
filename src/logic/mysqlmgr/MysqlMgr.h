#ifndef _SL_CORE_MYSQL_MGR_H_
#define _SL_CORE_MYSQL_MGR_H_
#include "IMysqlMgr.h"
#include <vector>
#include "sldb.h"
#include "slikernel.h"
#include "slsingleton.h"
#include <functional>
using namespace sl::db;

class TestHandler : public IMysqlHandler{
public:
	virtual bool onSuccess(sl::api::IKernel* pKernel, const int32 optType, const int32 affectedRow, const MysqlResult& result){
		return true;
	}
	virtual bool onFailed(sl::api::IKernel* pKernel, const int32 optType, const int32 errCode){
		return false;
	}
	virtual void onRelease(){ DEL this; }
};
class MysqlMgr : public IMysqlMgr, public sl::SLHolder<MysqlMgr>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void execSql(const int64 id, IMysqlHandler* handler, const SQLCommnandFunc& f);
	virtual void stopSql(IMysqlHandler* handler);

	static int32 escapeString(char* dest, const int32 destSize, const char* src, const int32 srcSize);

	void test();
private:
	sl::api::IKernel* _kernel;
	ISLDBConnectionPool* _dbConnectionPool;
	std::vector<ISLDBConnection*> _dbConnections;
	IMysqlBase* _handler;
	static ISLDBConnection* _escapeConnection;
};
#endif