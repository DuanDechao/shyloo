#include "sldb_task.h"
#include "sldb_engine.h"
#include "slkernel.h"
namespace sl
{
namespace core
{
DBTask::DBTask()
	:m_dbConnection(nullptr),
	m_dbTask(nullptr)
{}

DBTask::DBTask(api::IDBTask* pTask)
	:m_dbConnection(nullptr),
	m_dbTask(pTask)
{}

DBTask::~DBTask(){
	if (m_dbTask)
		m_dbTask->release();
	m_dbTask = nullptr;
}

bool DBTask::process(){
	m_dbConnection = DBEngine::getInstance()->allocDBConnecton();
	//获取连接失败
	if (!m_dbConnection)
		return false;

	SLASSERT(m_dbTask, "wtf");
	return m_dbTask->threadProcess(core::Kernel::getInstance(), m_dbConnection);
}

void DBTask::release(){
	RELEASE_POOL_OBJECT(DBTask, this);
}

thread::TPTaskState DBTask::presentMainThread(){
	if (!m_dbConnection)
		return thread::TPTaskState::TPTASK_STATE_CONTINUE_CHILDTHREAD;

	DBEngine::getInstance()->releaseDBConnecton(m_dbConnection);
	m_dbConnection = nullptr;

	return m_dbTask->mainThreadProcess(core::Kernel::getInstance());
}

DBTask* DBTask::newDBTask(api::IDBTask* pTask){
	return CREATE_POOL_OBJECT(DBTask, pTask);
}

}
}