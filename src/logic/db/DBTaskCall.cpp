#include "DBTaskCall.h"
#include "DB.h"
DBDataResult::DBDataResult(sl::db::ISLDBResult* pDBResult)
:m_pDBResult(pDBResult)
{}

DBDataResult::~DBDataResult(){
	if (m_pDBResult)
		m_pDBResult->release();
}

bool DBDataResult::next() const {
	return m_pDBResult->next();
}

int32 DBDataResult::count() const{
	return m_pDBResult->rowNum();
}

int8 DBDataResult::getDataInt8(const char* colName) const {
	int32 col = m_pDBResult->colNameToIdx(colName);
	SLASSERT(col >= 0, "invaild column name %s", colName);
	return m_pDBResult->toInt8(col);
}

int16 DBDataResult::getDataInt16(const char* colName) const {
	int32 col = m_pDBResult->colNameToIdx(colName);
	SLASSERT(col >= 0, "invaild column name %s", colName);
	return m_pDBResult->toInt16(col);
}

int32 DBDataResult::getDataInt32(const char* colName) const {
	int32 col = m_pDBResult->colNameToIdx(colName);
	SLASSERT(col >= 0, "invaild column name %s", colName);
	return m_pDBResult->toInt32(col);
}
int64 DBDataResult::getDataInt64(const char* colName) const {
	int32 col = m_pDBResult->colNameToIdx(colName);
	SLASSERT(col >= 0, "invaild column name %s", colName);
	return m_pDBResult->toInt64(col);
}

float DBDataResult::getDataFloat(const char* colName) const {
	int32 col = m_pDBResult->colNameToIdx(colName);
	SLASSERT(col >= 0, "invaild column name %s", colName);
	return m_pDBResult->toFloat(col);
}

const char * DBDataResult::getDataString(const char* colName) const{
	int32 col = m_pDBResult->colNameToIdx(colName);
	SLASSERT(col >= 0, "invaild column name %s", colName);
	return m_pDBResult->toString(col);
}

DBTaskCall::DBTaskCall()
	:m_pTask(nullptr),
	m_paramsBuf(nullptr),
	m_parBufSize(0),
	m_cb(nullptr)
{}


DBTaskCall::DBTaskCall(sl::api::IDBTask* pTask, DBTaskCallBackType cb, const char* pParamsBuf, int32 bufSize)
	:m_pTask(pTask),
	m_cb(cb),
	m_paramsBuf(pParamsBuf),
	m_parBufSize(bufSize)
{}

DBTaskCall::~DBTaskCall(){
	if (m_pTask)
		m_pTask->release();

	m_pTask = nullptr;
	FREE((void*)m_paramsBuf);
	m_paramsBuf = nullptr;
	m_parBufSize = 0;
}

DBTaskCall* DBTaskCall::newDBTaskCall(sl::api::IDBTask* pTask, DBTaskCallBackType cb, const OArgs& params){
	const int32 bufSize = params.getSize();
	const char* pParamsBuf = (const char*)MALLOC(bufSize);
	sl::SafeMemcpy((void *)pParamsBuf, bufSize, params.getContext(), bufSize);
	return CREATE_POOL_OBJECT(DBTaskCall, pTask, cb, pParamsBuf, bufSize);
}

bool DBTaskCall::threadProcess(sl::api::IKernel* pKernel, sl::db::ISLDBConnection* pDBConnection){
	OArgs args(m_paramsBuf, m_parBufSize);
	return m_pTask->threadProcess(pKernel, pDBConnection, args);
}

sl::thread::TPTaskState DBTaskCall::mainThreadProcess(sl::api::IKernel* pKernel) {
	sl::thread::TPTaskState _state = m_pTask->mainThreadProcess(pKernel);

	if (_state == sl::thread::TPTaskState::TPTASK_STATE_COMPLETED){
		DBDataResult dbResult(m_pTask->getTaskResult());
		if (m_cb != nullptr)
			m_cb(pKernel, dbResult);
	}
	return _state;
}

void DBTaskCall::release(){
	RELEASE_POOL_OBJECT(DBTaskCall, this);
}