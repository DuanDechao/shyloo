#ifndef __SL_CORE_DBMGR_H__
#define __SL_CORE_DBMGR_H__
#include "IDB.h"
#include <unordered_map>
#include "slikernel.h"
#include <list>
#include "slsingleton.h"

class IHarbor;
class IMysqlMgr;
class DataBase;
class DB : public IDB, public sl::SLHolder<DB> {
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

//	virtual IDBCall* create(int64 threadId, const int64 id, const char* file, const int32 line, const void* context, const int32 size = 0);
	//virtual IDBInterface* getDBInterface() {return _dbInterface;}

	IMysqlMgr* getMysqlMgr(){ return _mysql; }

	void test();

private:
	sl::api::IKernel*	_kernel;
	DB*					_self;
	IDBInterface*		_dbInterface;
	IHarbor*			_harbor;
	IMysqlMgr*			_mysql;
	DataBase*			_database;
};



#endif
