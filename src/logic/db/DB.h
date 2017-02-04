#ifndef __SL_CORE_DBMGR_H__
#define __SL_CORE_DBMGR_H__
#include "IDB.h"
#include <unordered_map>
#include "slobjectpool.h"
#include "slikernel.h"
#include <list>

class IHarbor;
class DB :public IDB
{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void execDBTask(sl::api::IDBTask* pTask, const OArgs& args, DBTaskCallBackType cb = nullptr);
private:
	sl::api::IKernel*	m_kernel;
	IHarbor*			m_harbor;
};



#endif