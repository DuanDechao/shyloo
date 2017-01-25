#ifndef __SL_CORE_DB_TASK_CALL_H__
#define __SL_CORE_DB_TASK_CALL_H__
#include "slmulti_sys.h"
#include "slikernel.h"
#include "slobjectpool.h"
#include "sldb.h"
class IDB;

class DBDataResult : public sl::api::ICacheDataResult{
public:
	DBDataResult(sl::db::ISLDBResult* pDBResult);
	~DBDataResult();
	virtual bool next() const;
	virtual int32 count() const;
	virtual int8 getDataInt8(const char* colName) const;
	virtual int16 getDataInt16(const char* colName) const;
	virtual int32 getDataInt32(const char* colName) const;
	virtual int64 getDataInt64(const char* colName) const;
	virtual float getDataFloat(const char* colName) const;
	virtual const char * getDataString(const char* colName) const;
	

private:
	sl::db::ISLDBResult*	m_pDBResult;
};


class DBTaskCall : public sl::api::IDBTask{
public:
	DBTaskCall();
	DBTaskCall(sl::api::IDBTask* pTask, int32 cbID);
	~DBTaskCall();

	static DBTaskCall* newDBTaskCall(sl::api::IDBTask* pTask, int32 cbID);

	virtual bool threadProcess(sl::api::IKernel* pKernel, sl::db::ISLDBConnection* pDBConnection);
	virtual sl::thread::TPTaskState mainThreadProcess(sl::api::IKernel* pKernel);
	virtual void release();

private:
	sl::api::IDBTask*		m_pTask;
	int32					m_cbID;
};

CREATE_OBJECT_POOL(DBTaskCall);

#endif