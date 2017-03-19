#ifndef SL_DB_DEFINE_H
#define SL_DB_DEFINE_H
#include "sldb.h"
#include "slmulti_sys.h"
#include "mysql.h"
#include <string>
namespace sl
{
namespace db
{
class DBConnectionInfo{
public:
	DBConnectionInfo()
		:m_hostName(""),
		m_userName(""),
		m_passwd(""),
		m_dbName(""),
		m_charSet(""),
		m_port(0)
	{}
	~DBConnectionInfo(){}

	const char* host() const { return m_hostName.c_str(); }
	const char* user() const { return m_userName.c_str(); }
	const char* pwd() const { return m_passwd.c_str(); }
	const char* database() const { return m_dbName.c_str(); }
	const char* charset() const { return m_charSet.c_str(); }
	const int32 port() const { return m_port; }

	void host(const char* str){ m_hostName = str; }
	void user(const char* str){ m_userName = str; }
	void pwd(const char* str){ m_passwd = str; }
	void database(const char* str){ m_dbName = str; }
	void charset(const char* str){ m_charSet = str; }
	void port(int32 iport) { m_port = iport; }
		
private:
	std::string		m_hostName;
	std::string		m_userName;
	std::string		m_passwd;
	std::string		m_dbName;
	std::string		m_charSet;
	int32			m_port;
};
}
}
#endif