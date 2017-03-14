#ifndef __SL_CORE_DBMGR_H__
#define __SL_CORE_DBMGR_H__
#include "IDB.h"
#include <unordered_map>
#include "slobjectpool.h"
#include "slikernel.h"
#include <list>
#include "slsingleton.h"
class IHarbor;
class IMysqlMgr;
class DB : public IDB, public sl::SLHolder<DB> {
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual IDBCall* create(int64 threadId, const int64 id, const char* file, const int32 line, const void* context, const int32 size = 0);

	IMysqlMgr* getMysqlMgr(){ return _mysql; }
private:
	sl::api::IKernel*	_kernel;
	DB*					_self;
	IHarbor*			_harbor;
	IMysqlMgr*			_mysql;
};



#endif