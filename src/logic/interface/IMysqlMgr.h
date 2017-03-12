#ifndef __SL_INTERFACE_MYSQL_MGR_H__
#define __SL_INTERFACE_MYSQL_MGR_H__
#include "slimodule.h"
#include <unordered_map>
#include <vector>
#include "slikernel.h"
#include "../mysqlmgr/SQLBase.h"

class ISQLBuilder{
public:
	virtual ISQLBuilder* table(const char* tableName) = 0;
	virtual ISQLBuilder* select(const char* field) = 0;
	virtual ISQLBuilder* where(const Expr& expr) = 0;
	virtual ISQLBuilder* orWhere(const Expr& expr) = 0;
	virtual ISQLBuilder* insert(const SetExpr* val) = 0;
	virtual ISQLBuilder* get(const int32 limit) = 0;
	virtual ISQLBuilder* update(const SetExpr* val) = 0;
	virtual ISQLBuilder* del() = 0;

	virtual int32 optType() const = 0;
	virtual const char* toString() = 0;
	virtual bool checkVaild() = 0;
	virtual bool submit() = 0;
};

class SQLCommand{
public:
	SQLCommand(ISQLBuilder* sqlBuilder) :_sqlBuilder(sqlBuilder){}
	~SQLCommand(){
		if (_sqlBuilder)
			DEL _sqlBuilder;

		_sqlBuilder = nullptr; 
	}
	SQLCommand& table(const char* tableName){
		_sqlBuilder->table(tableName);
		return *this;
	}

	SQLCommand& select(const char* field){
		_sqlBuilder->select(field);
		return *this;
	}
	SQLCommand& where(const Expr& expr){
		_sqlBuilder->where(expr);
		return *this;
	}
	SQLCommand& orWhere(const Expr& expr){
		_sqlBuilder->orWhere(expr);
		return *this;
	}
	SQLCommand& insert(const SetExpr* val){
		_sqlBuilder->insert(val);
		return *this;
	}
	SQLCommand& get(const int32 limit = 0){
		_sqlBuilder->get(limit);
		return *this;
	}
	SQLCommand& update(const SetExpr* val){
		_sqlBuilder->update(val);
		return *this;
	}
	SQLCommand& del(){
		_sqlBuilder->del();
		return *this;
	}


	int32 optType() const {
		return _sqlBuilder->optType();
	}
	const char* toString(){
		return _sqlBuilder->toString();
	}
	bool checkVaild(){
		return _sqlBuilder->checkVaild();
	}
	bool submit(){
		return _sqlBuilder->submit();
	}

private:
	ISQLBuilder* _sqlBuilder;
};

class IMysqlBase{
public:
	virtual ~IMysqlBase(){}
};

typedef std::vector<std::unordered_map<string,string>> MysqlResult;
class IMysqlHandler{
public:
	IMysqlHandler() :_base(nullptr){}
	virtual ~IMysqlHandler() {}

	inline void setBase(IMysqlBase* base) { _base = base; }
	inline IMysqlBase* getBase() const { return _base; }
	
	virtual bool onSuccess(sl::api::IKernel* pKernel, const int32 optType, const int32 affectedRow, const MysqlResult& result) = 0;
	virtual bool onFailed(sl::api::IKernel* pKernel, const int32 optType, const int32 errCode) = 0;
	virtual void onRelease() = 0;
private:
	IMysqlBase* _base;
};

typedef std::function<void(sl::api::IKernel* pKernel, SQLCommand& sqlCommnand)> SQLCommnandFunc;
class IMysqlMgr : public sl::api::IModule{
public:
	virtual  ~IMysqlMgr(){}

	virtual void execSql(const int64 id, IMysqlHandler* handler, SQLCommnandFunc& f) = 0;
	virtual void stopSql(IMysqlHandler* handler) = 0;
};
#endif