#include "sldb_connection.h"
#include "sldb_result.h"
namespace sl
{
namespace db
{
SLDBConnection::SLDBConnection(ISLDBConnectionPool* pConnPool){
	mysql_init(&m_mysqlHandler);
	m_pConnPool = pConnPool;
}

SLDBConnection::~SLDBConnection(){
	if (isActive())
		mysql_close(&m_mysqlHandler);
}

void SLDBConnection::release(){
	if (m_pConnPool)
		m_pConnPool->releaseConnection(this);
}

bool SLDBConnection::open(const char* szHostName, const int32 port, const char* szName, const char* szPwd, 
	const char* szDBName, const char* szCharSet){
	m_connInfo.host(szHostName);
	m_connInfo.user(szName);
	m_connInfo.pwd(szPwd);
	m_connInfo.database(szDBName);
	m_connInfo.charset(szCharSet);
	m_connInfo.port(port);
	
	return connectDBSvr();
}

bool SLDBConnection::reOpen(){
	mysql_close(&m_mysqlHandler);
	mysql_init(&m_mysqlHandler);

	return connectDBSvr();
}

ISLDBResult* SLDBConnection::executeWithResult(const char* commandSql){
	if (!isActive())
		return NULL;

	if (mysql_real_query(&m_mysqlHandler, commandSql, (unsigned long)strlen(commandSql))){
		SLASSERT(false, "mysql query sql:%s failed", commandSql);
		return NULL;
	}
	
	SLDBResult* res = SLDBResult::create();
	SLASSERT(res, "create SLDBResult failed!");
	res->setResult(m_mysqlHandler);
	
	return res;
}

bool SLDBConnection::execute(const char* commandSql){
	if (!isActive())
		return false;

	if (mysql_real_query(&m_mysqlHandler, commandSql, (unsigned long)strlen(commandSql))){
		SLASSERT(false, "mysql query sql:%s failed", commandSql);
		return false;
	}
	return true;
}

unsigned long SLDBConnection::escapeString(char* dest, const char* src, unsigned long srcSize){
	if (!isActive())
		return 0;

	return mysql_real_escape_string(&m_mysqlHandler, dest, src, srcSize);
}

unsigned int SLDBConnection::getLastErrno(){
	return mysql_errno(&m_mysqlHandler);
}

const char* SLDBConnection::getLastError(){
	return mysql_error(&m_mysqlHandler);
}

bool SLDBConnection::connectDBSvr(){
	int32 value = 1;
	mysql_options(&m_mysqlHandler, MYSQL_OPT_RECONNECT, (char*)&value);
	if (mysql_real_connect(&m_mysqlHandler, m_connInfo.host(), m_connInfo.user(), m_connInfo.pwd(),
		m_connInfo.database(), m_connInfo.port(), NULL, 0) == NULL){
		SLASSERT(false, "connect db server %s[%d] failed, error %s", m_connInfo.host(), m_connInfo.port(), mysql_error(&m_mysqlHandler));
		return false;
	}

	if (strcmp(m_connInfo.charset(), "") != 0){
		if (mysql_set_character_set(&m_mysqlHandler, m_connInfo.charset())){
			SLASSERT(false, "mysql_set_character_set %d failed!", m_connInfo.charset());
			return false;
		}
	}

	return true;
}

bool SLDBConnection::isActive(){
	return mysql_ping(&m_mysqlHandler) == 0;
}


}
}