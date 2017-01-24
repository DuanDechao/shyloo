#ifndef __SL_CORE_DB_ENGINE_H__
#define __SL_CORE_DB_ENGINE_H__
#include "slthread.h"
#include "sldb.h"

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

	virtual bool addDBTask(DBTask* pDBTask);
	virtual sl::db::ISLDBConnection* allocDBConnecton();
	virtual void releaseDBConnecton(db::ISLDBConnection* pConn);

private:
	DBEngine(){}
	~DBEngine();

private:
	sl::thread::ISLThreadPool*		m_threadPool;
	sl::db::ISLDBConnectionPool*	m_connectionPool;
};
}
}
#endif