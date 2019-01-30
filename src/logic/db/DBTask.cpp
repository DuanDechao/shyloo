#include "DBTask.h"
#include "DBTable.h"
bool DBQueryTask::onExecute(sl::api::IKernel* pKernel){
	if(dataBase() == NULL){
		SLASSERT(false, "DBQueryTask cant find database");
		return false;
	}
	
	if(_tableId <= 0 && _tableName == ""){
		SLASSERT(false, "wtf");
		return false;
	}

	if(_tableId > 0){
		DBTable* pTable = dataBase()->findTable(tableId());
		_tableName = pTable->tableName();
		return dataBase()->queryTable(tableId(), dbid(), _queryData);
	}
	else if(_tableName != ""){
		DBTable* pTable = dataBase()->findTable(tableName());
		_tableId = pTable->tableId();
		return dataBase()->queryTable(tableName(), dbid(), _queryData);
	}
	return false;
}

bool DBQueryTask::onSuccess(sl::api::IKernel* pKernel){
	if(_queryCB){
		sl::OBStream outData = _queryData.out();
		_queryCB(pKernel, true, tableName(), tableId(), outData);
	}
	return true;
}

bool DBQueryTask::onFailed(sl::api::IKernel* pKernel, bool nonviolent){
	if(_queryCB){
		sl::OBStream outData = _queryData.out();
		_queryCB(pKernel, false, tableName(), tableId(), outData);
	}
	return true;
}

bool DBWriteTask::onExecute(sl::api::IKernel* pKernel){
	if(dataBase() == NULL){
		SLASSERT(false, "DBQueryTask cant find database");
		return false;
	}
	
	if(_tableId <= 0 && _tableName == ""){
		SLASSERT(false, "wtf");
		return false;
	}

	sl::OBStream outData = _writeData.out();
	if(_tableId > 0){
		_insertId = dataBase()->writeTable(tableId(), dbid(), outData);
	}
	else if(_tableName != ""){
		_insertId = dataBase()->writeTable(tableName(), dbid(), outData);
	}
	return _insertId > 0;
}

bool DBWriteTask::onSuccess(sl::api::IKernel* pKernel){
	if(_writeCB){
		_writeCB(pKernel, true, tableName(), tableId(), _insertId);
	}
	return true;
}

bool DBWriteTask::onFailed(sl::api::IKernel* pKernel, bool nonviolent){
	if(_writeCB){
		_writeCB(pKernel, false, tableName(), tableId(), _insertId);
	}
	return true;
}
