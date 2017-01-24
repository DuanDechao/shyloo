#include "sldb_engine.h"
#include "slconfig_engine.h"
#include "slxml_reader.h"
namespace sl
{
namespace core
{
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
	m_connectionPool = sl::db::newDBConnectionPool(coreConf->sDBConnetionNum, host, port, user, pwd, dbName, charset);
	SLASSERT(m_connectionPool, "create connectionPool failed");
	return true;
}

bool DBEngine::destory(){
	m_threadPool->release();
	m_connectionPool->release();

	DEL this;
	return true;
}

bool DBEngine::addDBTask(){

}
sl::db::ISLDBConnection* DBEngine::allocDBConnecton(){

}
void DBEngine::releaseDBConnecton(db::ISLDBConnection* pConn){

}



}
}