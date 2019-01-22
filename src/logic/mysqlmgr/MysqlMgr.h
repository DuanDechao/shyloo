#ifndef _SL_CORE_MYSQL_MGR_H_
#define _SL_CORE_MYSQL_MGR_H_
#include "IMysqlMgr.h"
#include "slsingleton.h"
#include <map>
class DBInterface;
class MysqlMgr : public IMysqlMgr, public sl::SLHolder<MysqlMgr>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual IDBInterface* createDBInterface(const char* host, const int32 port, const char* user, const char* pwd, const char* dbName, const char* charset, int32 threadNum = 1);
	virtual void closeDBInterface(const char* host, const int32 port, const char* dbName);

	sl::api::IKernel* getKernel() {return _kernel;}

private:
	sl::api::IKernel* _kernel;
	std::map<std::string, DBInterface*> _dbInterfaces;
};
#endif
