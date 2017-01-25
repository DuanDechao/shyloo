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
	m_cbID(-1)
{}


DBTaskCall::DBTaskCall(sl::api::IDBTask* pTask, int32 cbID)
	:m_pTask(pTask),
	m_cbID(cbID)
{}

DBTaskCall::~DBTaskCall(){
	if (m_pTask)
		m_pTask->release();

	m_pTask = nullptr;
}

DBTaskCall* DBTaskCall::newDBTaskCall(sl::api::IDBTask* pTask, int32 cbID){
	return CREATE_POOL_OBJECT(DBTaskCall, pTask, cbID);
}

bool DBTaskCall::threadProcess(sl::api::IKernel* pKernel, sl::db::ISLDBConnection* pDBConnection){
	return m_pTask->threadProcess(pKernel, pDBConnection);
}

sl::thread::TPTaskState DBTaskCall::mainThreadProcess(sl::api::IKernel* pKernel) {
	sl::thread::TPTaskState _state = m_pTask->mainThreadProcess(pKernel);

	if (_state == sl::thread::TPTaskState::TPTASK_STATE_COMPLETED){
		DBDataResult dbResult(m_pTask->getDBResult());
		DB::dealTaskCompleteCB(pKernel, m_cbID, dbResult);
	}
	return _state;
}

void DBTaskCall::release(){
	RELEASE_POOL_OBJECT(DBTaskCall, this);
}