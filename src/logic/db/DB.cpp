#include "DB.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "DBTaskCall.h"

DB::TASK_CALL_TYPE DB::m_dbTaskCompleteDB;

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

void DB::execDBTask(sl::api::IDBTask* pTask, int32 cbID){
	if (!pTask)
		return;

	DBTaskCall* pTaskCall = DBTaskCall::newDBTaskCall(pTask, cbID);
	m_kernel->addDBTask(pTaskCall);
}

void DB::rgsDBTaskCallBack(int32 messageId, sl::api::ICacheDataResult::DataReadFuncType cb){
	m_dbTaskCompleteDB[messageId].push_back(cb);
}

void DB::dealTaskCompleteCB(sl::api::IKernel * pKernel, int32 cbID, const sl::api::ICacheDataResult& result){
	if (cbID == INVAILD_CB_ID)
		return;

	auto itor = m_dbTaskCompleteDB.find(cbID);
	if (itor == m_dbTaskCompleteDB.end()){
		SLASSERT(false, "not have cb id %d", cbID);
		return;
	}

	for (auto& cb : itor->second){
		cb(pKernel, result);
	}
}


