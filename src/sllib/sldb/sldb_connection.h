#ifndef __SL_DB_CONNECTION_H__
#define __SL_DB_CONNECTION_H__
#include "sldb_define.h"

namespace sl
{
namespace db
{
class SLDBConnection : public ISLDBConnection{
public:
	SLDBConnection(ISLDBConnectionPool* pConnPool);
    virtual	~SLDBConnection();
	virtual bool open(const char* szHostName, const int32 port, const char* szName, const char* szPwd, const char* szDBName, const char* szCharSet);
	virtual bool SLAPI reOpen();
	virtual ISLDBResult* SLAPI executeWithResult(const char* commandSql);
	virtual bool SLAPI execute(const char* commandSql);
	virtual unsigned int SLAPI getLastErrno(void);
	virtual const char* SLAPI getLastError(void);
	virtual void SLAPI release(void);
	virtual unsigned long SLAPI escapeString(char* dest, const char* src, unsigned long srcSize);

	bool connectDBSvr();
	bool isActive();

private:
	DBConnectionInfo		m_connInfo;
	MYSQL					m_mysqlHandler;
	ISLDBConnectionPool*	m_pConnPool;
};

}
}

#endif
