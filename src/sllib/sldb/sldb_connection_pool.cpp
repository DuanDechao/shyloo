#define SL_DLL_EXPORT
#include "sldb_connection_pool.h"
#define MAX_CONNECTION_NUM 10
namespace sl
{
namespace db
{
SLDBConnectionPool::SLDBConnectionPool(int32 maxDBConnectionNum, const char* szHostName, const int32 port, const char* szName, const char* szPwd,
		const char* szDBName, const char* szCharSet)
	:m_maxConnectionNum(maxDBConnectionNum > MAX_CONNECTION_NUM ? MAX_CONNECTION_NUM : maxDBConnectionNum)
{
	m_connInfo.host(szHostName);
	m_connInfo.user(szName);
	m_connInfo.pwd(szPwd);
	m_connInfo.database(szDBName);
	m_connInfo.charset(szCharSet);
	m_connInfo.port(port);

	//InitializeCriticalSection(&m_allocConnectionMutex);
}

SLDBConnectionPool::~SLDBConnectionPool(){
	if (m_allConns.size() > 0){
		auto itor = m_allConns.begin();
		for (; itor != m_allConns.end(); ++itor){
			if (*itor)
				DEL *itor;
		}
	}
	m_allConns.clear();
	m_freeConns.clear();
	m_maxConnectionNum = 0;

	//DeleteCriticalSection(&m_allocConnectionMutex);
}

void SLDBConnectionPool::release(){
	DEL this;
}

ISLDBConnection* SLDBConnectionPool::allocConnection(){
	//EnterCriticalSection(&m_allocConnectionMutex);
	
	SLDBConnection* pConnection = NULL;
	if (m_freeConns.empty() && (int32)m_allConns.size() <= m_maxConnectionNum){
		pConnection = NEW SLDBConnection(this);
		SLASSERT(pConnection, "new db connection failed");

		pConnection->open(m_connInfo.host(), m_connInfo.port(), m_connInfo.user(), 
			m_connInfo.pwd(), m_connInfo.database(), m_connInfo.charset());

		m_allConns.push_back(pConnection);
	}
	else{
		pConnection = m_freeConns.front();
		m_freeConns.pop_front();	
	}

	//LeaveCriticalSection(&m_allocConnectionMutex);
	return pConnection;
}

void SLDBConnectionPool::releaseConnection(ISLDBConnection* pConn){
	if (!pConn)
		return;

	if (!hasConnection(pConn)){
		SLASSERT(false, "where connection %p from???", pConn);
		return;
	}

	auto itor = m_freeConns.begin();
	for (; itor != m_freeConns.end(); ++itor){
		if (*itor == pConn){
			SLASSERT(false, "connection %p has in free list", pConn);
		    return;
		}
	}
	m_freeConns.push_back((SLDBConnection*)pConn);
}

bool SLDBConnectionPool::hasConnection(ISLDBConnection* pConn){
	auto itor = m_allConns.begin();
	for (; itor != m_allConns.end(); ++itor){
		if (*itor == pConn)
			return true;
	}
	return false;
}

extern "C" SL_DLL_API sl::db::ISLDBConnectionPool* SLAPI newDBConnectionPool(int32 maxConnectionNum, const char* szHostName, const int32 port, const char* szName, const char* szPwd,
	const char* szDBName, const char* szCharSet){
	return NEW sl::db::SLDBConnectionPool(maxConnectionNum, szHostName, port, szName, szPwd, szDBName, szCharSet);
}

}
}

