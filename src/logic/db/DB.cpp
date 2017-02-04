#include "DB.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "DBTaskCall.h"

bool DB::initialize(sl::api::IKernel * pKernel){
	m_kernel = pKernel;
	return true;
}

bool DB::launched(sl::api::IKernel * pKernel){
	m_harbor = (IHarbor*)pKernel->findModule("Harbor");
	SLASSERT(m_harbor, "not find module harbor");

	return true;
}

bool DB::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void DB::execDBTask(sl::api::IDBTask* pTask, const OArgs& args, DBTaskCallBackType cb){
	if (!pTask)
		return;

	DBTaskCall* pTaskCall = DBTaskCall::newDBTaskCall(pTask, cb, args);
	m_kernel->addDBTask(pTaskCall);
}

