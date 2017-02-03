#ifndef __SL_CORE_DB_ENGINE_H__
#define __SL_CORE_DB_ENGINE_H__
#include "slthread.h"
#include "sldb.h"
#include "slikernel.h"
namespace sl
{
namespace core
{
class DBTask;
class DBEngine{
public:
	static DBEngine* getInstance();
	virtual bool ready();
	virtual bool initialize();
	virtual bool destory();

	virtual bool addDBTaskCall(api::IDBTaskCall* pDBTaskCall);
	virtual sl::db::ISLDBConnection* allocDBConnecton();
	virtual void releaseDBConnecton(db::ISLDBConnection* pConn);
	virtual int64 processing(int64 overTime);

private:
	DBEngine();
	~DBEngine();

private:
	sl::thread::ISLThreadPool*		m_threadPool;
	sl::db::ISLDBConnectionPool*	m_connectionPool;
};
}
}
#endif