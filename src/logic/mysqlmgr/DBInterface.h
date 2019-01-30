#ifndef __SL_DB_INTERFACE_H__
#define __SL_DB_INTERFACE_H__
#include <vector>
#include "sldb.h"
#include "slikernel.h"
#include <functional>
#include "IMysqlMgr.h"
using namespace sl::db;

class DBInterface: public IDBInterface{
public:
	static DBInterface* create(const char* host, const int32 port, const char* user, const char* pwd, const char* dbName, const char* charset){
		return NEW DBInterface(host, port, user, pwd, dbName, charset);
	}

	virtual void release(){
		DEL this;
	}
	
	virtual bool execSql(const int64 id, const SQLCommnandFunc& f, const SQLExecCallback& cb);
	virtual bool execSql(const int64 id, const int32 optType, const char* sql, const SQLExecCallback& cb);
	virtual bool execSql(const int64 id, SQLCommand* sqlCommand, const SQLExecCallback& cb);
	virtual IMysqlResult* execSqlSync(const int64 id, const SQLCommnandFunc& f);
	virtual IMysqlResult* execSqlSync(const int64 id, const int32 optType, const char* sql);
	virtual IMysqlResult* execSqlSync(const int64 id, SQLCommand* sqlCommand);
	virtual IMysqlResult* getTableFields(const int64 id, const char* tableName);
	virtual SQLCommand* createSqlCommand(const int64 id);
	virtual const char* host() {return _host.c_str();}
	virtual const int32 port() {return _port;}
	virtual const char* user() {return _user.c_str();}
	virtual const char* passwd() {return _pwd.c_str();}
	virtual const char* dbName() {return _dbName.c_str();}
	virtual const char* charset() {return _charset.c_str();}
	virtual const int32 connectNum() {return _connectNum;}
	
	void test();

private:
	DBInterface(const char* host, const int32 port, const char* user, const char* pwd, const char* dbName, const char* charset);
	virtual ~DBInterface();

private:
	std::string						_host;				//db主机
	int32							_port;				//db端口
	std::string						_user;				//用户名
	std::string						_pwd;				//密码
	std::string						_dbName;			//数据库名
	std::string						_charset;			//字符集
	int32							_connectNum;		//连接数
	ISLDBConnectionPool*			_dbConnectionPool;	//连接池
	std::vector<ISLDBConnection*>	_dbConnections;		//连接对象集
};
#endif
