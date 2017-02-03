#include "sldb_engine.h"
#include "slconfig_engine.h"
#include "slxml_reader.h"
#include "sldb_task.h"
namespace sl
{
namespace core
{
DBEngine::DBEngine() 
	:m_threadPool(nullptr), 
	 m_connectionPool(nullptr)
{}

DBEngine::~DBEngine(){}

DBEngine* DBEngine::getInstance(){
	static DBEngine* p = nullptr;
	if (!p){
		p = NEW DBEngine;
		if (!p->ready()){
			SLASSERT(false, "DB Engine not ready");
			DEL p;
			p = nullptr;
		}
	}
	return p;
}

bool DBEngine::ready(){
	return true;
}

bool DBEngine::initialize(){
	const sCoreConfig* coreConf = ConfigEngine::getInstance()->getCoreConfig();
	SLASSERT(coreConf, "wttf");
	if (!coreConf->sOpenDBSvr)
		return true;

	m_threadPool = sl::thread::createThreadPool(2, coreConf->sDBMinWorkThreadNum, coreConf->sDBMaxWorkThreadNum);
	SLASSERT(m_threadPool, "create thread pool failed");
	if (!m_threadPool->start()){
		SLASSERT(false, "start thread pool failed");
		return false;
	}

	sl::XmlReader svrConf;
	if (!svrConf.loadXml(ConfigEngine::getInstance()->getCoreFile())){
		SLASSERT(false, "cant load core file");
		return false;
	}
	const char* host = svrConf.root()["db"][0].getAttributeString("host");
	const int32 port = svrConf.root()["db"][0].getAttributeInt32("port");
	const char* user = svrConf.root()["db"][0].getAttributeString("user");
	const char* pwd = svrConf.root()["db"][0].getAttributeString("pwd");
	const char* dbName = svrConf.root()["db"][0].getAttributeString("db");
	const char* charset = svrConf.root()["db"][0].getAttributeString("charset");
	m_connectionPool = db::newDBConnectionPool(coreConf->sDBConnetionNum, host, port, user, pwd, dbName, charset);
	SLASSERT(m_connectionPool, "create connectionPool failed");
	return true;
}

bool DBEngine::destory(){
	if (m_threadPool)
		m_threadPool->release();

	if (m_connectionPool)
		m_connectionPool->release();

	DEL this;
	return true;
}

bool DBEngine::addDBTaskCall(api::IDBTaskCall* pTaskCall){
	if (m_threadPool){
		DBTask* pDBTask = DBTask::newDBTaskCall(pTaskCall);
		SLASSERT(pDBTask, "wtf");
		return m_threadPool->addTask(pDBTask);
	}
		
	return false;
}

sl::db::ISLDBConnection* DBEngine::allocDBConnecton(){
	if (m_connectionPool)
		return m_connectionPool->allocConnection();

	return nullptr;
}

void DBEngine::releaseDBConnecton(db::ISLDBConnection* pConn){
	if (pConn && m_connectionPool)
		m_connectionPool->releaseConnection(pConn);
}

int64 DBEngine::processing(int64 overTime){
	if (!ConfigEngine::getInstance()->getCoreConfig()->sOpenDBSvr)
		return 0;

	int64 startTime = sl::getTimeMilliSecond();
	
	if (m_threadPool)
		m_threadPool->run(overTime);
	
	return sl::getTimeMilliSecond() - startTime;
}



}
}