#ifndef __SL_DB_INTERFACE_H__
#define __SL_DB_INTERFACE_H__
#include <vector>
#include "sldb.h"
#include "slikernel.h"
#include <functional>
#include "IMysqlMgr.h"
using namespace sl::db;

class TestHandler : public IMysqlHandler{
public:
	virtual bool onSuccess(sl::api::IKernel* pKernel, const int32 optType, const int32 affectedRow, IMysqlResult* result){
		return true;
	}
	virtual bool onFailed(sl::api::IKernel* pKernel, const int32 optType, const int32 errCode){
		return false;
	}
	virtual void onRelease(){ DEL this; }
};

class DBInterface: public IDBInterface{
public:
	static DBInterface* create(const char* host, const int32 port, const char* user, const char* pwd, const char* dbName, const char* charset, int32 threadNum){
		return NEW DBInterface(host, port, user, pwd, dbName, charset, threadNum);
	}

	void release(){
		DEL this;
	}
	
	virtual void execSql(const int64 id, IMysqlHandler* handler, const SQLCommnandFunc& f);
	virtual void execSql(const int64 id, IMysqlHandler* handler, const int32 optType, const char* sql);
	virtual void stopSql(IMysqlHandler* handler);
	virtual IMysqlResult* execSqlSync(const SQLCommnandFunc& f);
	virtual IMysqlResult* execSqlSync(const int32 optType, const char* sql);
	virtual const char* host() {return _host.c_str();}
	virtual const int32 port() {return _port;}
	virtual const char* user() {return _user.c_str();}
	virtual const char* passwd() {return _pwd.c_str();}
	virtual const char* dbName() {return _dbName.c_str();}
	virtual const char* charset() {return _charset.c_str();}
	virtual const int32 threadCount() {return _threadCount;}
	virtual IMysqlResult* getTableFields(const char* tableName);
	
	int32 escapeString(char* dest, const int32 destSize, const char* src, const int32 srcSize);

	void test();

private:
	DBInterface(const char* host, const int32 port, const char* user, const char* pwd, const char* dbName, const char* charset, int32 threadNum);
	virtual ~DBInterface();

private:
	std::string						_host;				//db主机
	int32							_port;				//db端口
	std::string						_user;				//用户名
	std::string						_pwd;				//密码
	std::string						_dbName;			//数据库名
	std::string						_charset;			//字符集
	int32							_threadCount;		//连接线程数
	ISLDBConnectionPool*			_dbConnectionPool;	//连接池
	std::vector<ISLDBConnection*>	_dbConnections;		//连接对象集
	ISLDBConnection*				_escapeConnection;  //做escape的专用连接
	ISLDBConnection*				_syncConnection;    //同步操作用的连接
};
#endif
