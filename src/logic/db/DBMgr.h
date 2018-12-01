#ifndef __SL_CORE_DBMGR_H__
#define __SL_CORE_DBMGR_H__
#include "IDB.h"
#include <unordered_map>
#include "slobjectpool.h"
class IHarbor;
class OArgs;

class DB :public IDB
{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void execDBTask(int32 nodeType, int32 nodeId, int32 cbId, sl::api::IDBTask* pTask, bool isCallBack = true);

	virtual void DBTaskCallBack();

private:
	static sl::api::IKernel*	s_kernel;
	static IHarbor*				s_harbor;
};



#endif
