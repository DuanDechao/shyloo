#ifndef __SL_CORE_DBCREATOR_DBCALL_H__
#define __SL_CORE_DBCREATOR_DBCALL_H__
#include "IMysqlMgr.h"
#include "slikernel.h"
typedef std::function<void(sl::api::IKernel* pKernel, const char* tableName, const bool success, IMysqlResult* result)> DBCallBack;
class DBCall: public IMysqlHandler{
public:
	DBCall(string tableName, int64 threadId);
	~DBCall();

	void exec(const int8 optType, const char* sql, const DBCallBack& cb);
	virtual bool onSuccess(sl::api::IKernel* pKernel, const int32 optType, const int32 affectedRow, IMysqlResult* result);
	virtual bool onFailed(sl::api::IKernel* pKernel, const int32 optType, const int32 errCode);
	virtual void onRelease();

protected:
	string		_tableName;
	DBCallBack	_cb;
	int64		_threadId;
};

#endif