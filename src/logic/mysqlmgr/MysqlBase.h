#ifndef __SL_CORE_MYSQL_BASE_H__
#define __SL_CORE_MYSQL_BASE_H__
#include "IMysqlMgr.h"
#include "sldb.h"
using namespace sl::db;
class MysqlBase : public IMysqlBase, public sl::api::IAsyncHandler{
public:
	MysqlBase(ISLDBConnection* dbConn, SQLCommand* sqlCommand);
	~MysqlBase();

	virtual bool onExecute(sl::api::IKernel* pKernel);
	virtual bool onSuccess(sl::api::IKernel* pKernel);
	virtual bool onFailed(sl::api::IKernel* pKernel, bool nonviolent);
	virtual void onRelease(sl::api::IKernel* pKernel);

	void Exec(IMysqlHandler* handler);
private:
	ISLDBConnection* _dbConnection;
	IMysqlHandler* _handler;
	SQLCommand* _sqlCommand;
	MysqlResult _result;
	int32 _errCode;
	int32 _affectedRow;
};
#endif