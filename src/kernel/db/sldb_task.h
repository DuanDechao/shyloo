#ifndef __SL_CORE_DB_TASK_H__
#define __SL_CORE_DB_TASK_H__
#include "slthread.h"
#include "sldb.h"
namespace sl
{
namespace core
{
class DBTask : public sl::thread::ITPTask{
public:
	DBTask();
	~DBTask();
	virtual bool threadProcess(db::ISLDBConnection* pDBConnection) = 0;
	virtual thread::TPTaskState mainThreadProcess() = 0;
	virtual bool start(){}
	virtual bool process();
	virtual bool end(){}
	virtual thread::TPTaskState presentMainThread();
	
private:
	sl::db::ISLDBConnection*	m_dbConnection;
};
}
}
#endif