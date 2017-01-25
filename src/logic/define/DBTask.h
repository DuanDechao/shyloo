#ifndef __SL_DEFINE_DB_TASK_H__
#define __SL_DEFINE_DB_TASK_H__
#include "slikernel.h"
#include "sldb.h"
using namespace sl;

class QueryAccountDBTask : public sl::api::IDBTask
{
public:
	static sl::api::IDBTask* newObj(){ return NEW QueryAccountDBTask(); }

	virtual bool threadProcess(sl::api::IKernel* pKernel, sl::db::ISLDBConnection* pDBConnection){
		return true;
	}
	virtual thread::TPTaskState mainThreadProcess(sl::api::IKernel* pKernel){
		return 0;
	}
};

#endif