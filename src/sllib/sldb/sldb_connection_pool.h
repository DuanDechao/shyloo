#ifndef __SL_LIB_DB_CONNECTION_POOL_H__
#define __SL_LIB_DB_CONNECTION_POOL_H__
#include "sldb_connection.h"
#include <list>
namespace sl
{
namespace db
{
class SLDBConnectionPool : public ISLDBConnectionPool{
public:
	SLDBConnectionPool(int32 maxDBConnectionNum, const char* szHostName, const int32 port, const char* szName, const char* szPwd, 
		const char* szDBName, const char* szCharSet);
	virtual ~SLDBConnectionPool();

	virtual ISLDBConnection* SLAPI allocConnection();
	virtual void SLAPI releaseConnection(ISLDBConnection* pConn);
	virtual void SLAPI release(void);

	bool hasConnection(ISLDBConnection* pConn);

private:
	DBConnectionInfo				m_connInfo;
	int32							m_maxConnectionNum;
	std::list<SLDBConnection*>		m_allConns;
	std::list<SLDBConnection*>		m_freeConns;

	//CRITICAL_SECTION				m_allocConnectionMutex;	//∑÷≈‰connection ±µƒª•≥‚À¯
};
}
}
#endif

