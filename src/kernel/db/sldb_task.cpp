#include "sldb_task.h"
#include "sldb_engine.h"
#include "slkernel.h"
namespace sl
{
namespace core
{
DBTask::DBTask()
	:m_dbConnection(nullptr),
	m_dbTaskCall(nullptr)
{}

DBTask::DBTask(api::IDBTaskCall* pTaskCall)
	:m_dbConnection(nullptr),
	m_dbTaskCall(pTaskCall)
{}

DBTask::~DBTask(){
	if (m_dbTaskCall)
		m_dbTaskCall->release();
	m_dbTaskCall = nullptr;
}

bool DBTask::process(){
	m_dbConnection = DBEngine::getInstance()->allocDBConnecton();
	//获取连接失败
	if (!m_dbConnection)
		return false;

	SLASSERT(m_dbTaskCall, "wtf");
	return m_dbTaskCall->threadProcess(core::Kernel::getInstance(), m_dbConnection);
}

void DBTask::release(){
	RELEASE_POOL_OBJECT(DBTask, this);
}

thread::TPTaskState DBTask::presentMainThread(){
	if (!m_dbConnection)
		return thread::TPTaskState::TPTASK_STATE_CONTINUE_CHILDTHREAD;

	DBEngine::getInstance()->releaseDBConnecton(m_dbConnection);
	m_dbConnection = nullptr;

	return m_dbTaskCall->mainThreadProcess(core::Kernel::getInstance());
}

DBTask* DBTask::newDBTaskCall(api::IDBTaskCall* pTaskCall){
	return CREATE_POOL_OBJECT(DBTask, pTaskCall);
}

}
}