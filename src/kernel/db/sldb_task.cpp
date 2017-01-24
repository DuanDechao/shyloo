#include "sldb_task.h"
#include "sldb_engine.h"
namespace sl
{
namespace core
{

DBTask::DBTask() :m_dbConnection(nullptr){}

bool DBTask::process(){
	m_dbConnection = DBEngine::getInstance()->allocDBConnecton();
	//获取连接失败
	if (!m_dbConnection)
		return false;

	return threadProcess(m_dbConnection);
}

thread::TPTaskState DBTask::presentMainThread(){
	if (!m_dbConnection)
		return thread::TPTaskState::TPTASK_STATE_CONTINUE_CHILDTHREAD;

	DBEngine::getInstance()->releaseDBConnecton(m_dbConnection);
	m_dbConnection = nullptr;

	return mainThreadProcess();
}
}
}