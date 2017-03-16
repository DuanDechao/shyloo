#include "DBCall.h"
#include "DB.h"

class DBCallCondition : public IDBCallCondition{
public:
	DBCallCondition(SQLCommand& sqlCommand):_sqlCommand(sqlCommand) {}

	virtual void AddCondition(const char* key, const DBConditionOpType opt, const int8 value){
		switch (opt){
		case DBConditionOpType::DBOP_EQ: _sqlCommand.where(Field(key) == value); break;
		case DBConditionOpType::DBOP_NE: _sqlCommand.where(Field(key) != value); break;
		case DBConditionOpType::DBOP_GT: _sqlCommand.where(Field(key) > value); break;
		case DBConditionOpType::DBOP_GE: _sqlCommand.where(Field(key) >= value); break;
		case DBConditionOpType::DBOP_LS: _sqlCommand.where(Field(key) < value); break;
		case DBConditionOpType::DBOP_LE: _sqlCommand.where(Field(key) <= value); break;
		}
	}

	virtual void AddCondition(const char* key, const DBConditionOpType opt, const int16 value){
		switch (opt){ 
		case DBConditionOpType::DBOP_EQ: _sqlCommand.where(Field(key) == value); break;
		case DBConditionOpType::DBOP_NE: _sqlCommand.where(Field(key) != value); break;
		case DBConditionOpType::DBOP_GT: _sqlCommand.where(Field(key) > value); break;
		case DBConditionOpType::DBOP_GE: _sqlCommand.where(Field(key) >= value); break;
		case DBConditionOpType::DBOP_LS: _sqlCommand.where(Field(key) < value); break;
		case DBConditionOpType::DBOP_LE: _sqlCommand.where(Field(key) <= value); break;
		}
	}

	virtual void AddCondition(const char* key, const DBConditionOpType opt, const int32 value){
		switch (opt){
		case DBConditionOpType::DBOP_EQ: _sqlCommand.where(Field(key) == value); break;
		case DBConditionOpType::DBOP_NE: _sqlCommand.where(Field(key) != value); break;
		case DBConditionOpType::DBOP_GT: _sqlCommand.where(Field(key) > value); break;
		case DBConditionOpType::DBOP_GE: _sqlCommand.where(Field(key) >= value); break;
		case DBConditionOpType::DBOP_LS: _sqlCommand.where(Field(key) < value); break;
		case DBConditionOpType::DBOP_LE: _sqlCommand.where(Field(key) <= value); break;
		}
	}

	virtual void AddCondition(const char* key, const DBConditionOpType opt, const int64 value){
		switch (opt){
		case DBConditionOpType::DBOP_EQ: _sqlCommand.where(Field(key) == value); break;
		case DBConditionOpType::DBOP_NE: _sqlCommand.where(Field(key) != value); break;
		case DBConditionOpType::DBOP_GT: _sqlCommand.where(Field(key) > value); break;
		case DBConditionOpType::DBOP_GE: _sqlCommand.where(Field(key) >= value); break;
		case DBConditionOpType::DBOP_LS: _sqlCommand.where(Field(key) < value); break;
		case DBConditionOpType::DBOP_LE: _sqlCommand.where(Field(key) <= value); break;
		}
	}

	virtual void AddCondition(const char* key, const DBConditionOpType opt, const char* value){
		switch (opt){
		case DBConditionOpType::DBOP_EQ: _sqlCommand.where(Field(key) == value); break;
		case DBConditionOpType::DBOP_NE: _sqlCommand.where(Field(key) != value); break;
		case DBConditionOpType::DBOP_GT: _sqlCommand.where(Field(key) > value); break;
		case DBConditionOpType::DBOP_GE: _sqlCommand.where(Field(key) >= value); break;
		case DBConditionOpType::DBOP_LS: _sqlCommand.where(Field(key) < value); break;
		case DBConditionOpType::DBOP_LE: _sqlCommand.where(Field(key) <= value); break;
		}
	}

private:
	SQLCommand& _sqlCommand;
};

class DBQueryParamAdder: public IDBQueryParamAdder{
public:
	DBQueryParamAdder(SQLCommand& sqlCommand) :_sqlCommand(sqlCommand){}
	
	virtual void AddColumn(const char* key) { _sqlCommand.select(key); }

private:
	SQLCommand& _sqlCommand;
};

class DBInsertParamAdder : public IDBInsertParamAdder{
public:
	DBInsertParamAdder(SQLCommand& sqlCommand) :_sqlCommand(sqlCommand){}

	virtual void AddColumn(const char* key, const int8 value) { _sqlCommand.insert(Field(key) = value); }
	virtual void AddColumn(const char* key, const int16 value) { _sqlCommand.insert(Field(key) = value); }
	virtual void AddColumn(const char* key, const int32 value) { _sqlCommand.insert(Field(key) = value); }
	virtual void AddColumn(const char* key, const int64 value) { _sqlCommand.insert(Field(key) = value); }
	virtual void AddColumn(const char* key, const char* value) { _sqlCommand.insert(Field(key) = value); }
	virtual void AddColumn(const char* key, const void* value, const int32 size) { _sqlCommand.insert(Field(key).addStruct(value, size)); }

private:
	SQLCommand& _sqlCommand;
};

class DBUpdateParamAdder : public IDBUpdateParamAdder{
public:
	DBUpdateParamAdder(SQLCommand& sqlCommand) :_sqlCommand(sqlCommand){}

	virtual void AddColumn(const char* key, const int8 value) { _sqlCommand.update(Field(key) = value); }
	virtual void AddColumn(const char* key, const int16 value) { _sqlCommand.update(Field(key) = value); }
	virtual void AddColumn(const char* key, const int32 value) { _sqlCommand.update(Field(key) = value); }
	virtual void AddColumn(const char* key, const int64 value) { _sqlCommand.update(Field(key) = value); }
	virtual void AddColumn(const char* key, const char* value) { _sqlCommand.update(Field(key) = value); }
	virtual void AddColumn(const char* key, const void* value, const int32 size) { _sqlCommand.update(Field(key).addStruct(value, size)); }

private:
	SQLCommand& _sqlCommand;
};


void DBCall::query(const char* tableName, const DBQueryCommandFunc& f, const DBCallBack& cb){
	_cb = cb;
	
	_db->getMysqlMgr()->execSql(_threadId, this, [tableName, &f](sl::api::IKernel* pKernel, SQLCommand& sqlCommand){
		sqlCommand.table(tableName);
		
		if (f){
			DBQueryParamAdder adder(sqlCommand);
			DBCallCondition condition(sqlCommand);
			f(pKernel, &adder, &condition);
		}

		sqlCommand.get();
	});
}

void DBCall::insert(const char* tableName, const DBInsertCommandFunc& f, const DBCallBack& cb){
	_cb = cb;

	_db->getMysqlMgr()->execSql(_threadId, this, [tableName, &f](sl::api::IKernel* pKernel, SQLCommand& sqlCommand){
		sqlCommand.table(tableName);

		if (f){
			DBInsertParamAdder adder(sqlCommand);
			f(pKernel, &adder);
		}
	});
}

void DBCall::update(const char* tableName, const DBUpdateCommandFunc& f, const DBCallBack& cb){
	_cb = cb;

	_db->getMysqlMgr()->execSql(_threadId, this, [tableName, &f](sl::api::IKernel* pKernel, SQLCommand& sqlCommand){
		sqlCommand.table(tableName);

		if (f){
			DBUpdateParamAdder adder(sqlCommand);
			DBCallCondition condition(sqlCommand);
			f(pKernel, &adder, &condition);
		}
	});
}

void DBCall::del(const char* tableName, const DBDeleteCommandFunc& f, const DBCallBack& cb){
	_cb = cb;

	_db->getMysqlMgr()->execSql(_threadId, this, [tableName, &f](sl::api::IKernel* pKernel, SQLCommand& sqlCommand){
		sqlCommand.table(tableName);

		if (f){
			DBCallCondition condition(sqlCommand);
			f(pKernel, &condition);
		}

		sqlCommand.del();
	});
}

bool DBCall::onSuccess(sl::api::IKernel* pKernel, const int32 optType, const int32 affectedRow, const MysqlResult& result){
	if (_cb){
		DBResult res(result);
		_cb(pKernel, _id, true, affectedRow, this, &res);
	}

	return true;
}

bool DBCall::onFailed(sl::api::IKernel* pKernel, const int32 optType, const int32 errCode){
	if (_cb)
		_cb(pKernel, _id, false, 0, this, nullptr);

	return true;
}

void DBCall::onRelease(){
	release();
}

