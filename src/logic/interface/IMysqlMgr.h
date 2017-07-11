#ifndef __SL_INTERFACE_MYSQL_MGR_H__
#define __SL_INTERFACE_MYSQL_MGR_H__
#include "slimodule.h"
#include <unordered_map>
#include <vector>
#include "slikernel.h"
#include "SQLBase.h"

class ISQLBuilder{
public:
	virtual ~ISQLBuilder() {}
	virtual ISQLBuilder* table(const char* tableName) = 0;
	virtual ISQLBuilder* select(const char* field) = 0;
	virtual ISQLBuilder* where(const Expr& expr) = 0;
	virtual ISQLBuilder* orWhere(const Expr& expr) = 0;
	virtual ISQLBuilder* insert(const SetExpr* val) = 0;
	virtual ISQLBuilder* get(const int32 limit) = 0;
	virtual ISQLBuilder* update(const SetExpr* val) = 0;
	virtual ISQLBuilder* save(const SetExpr* val) = 0;
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

	int32 optType() const { return _sqlBuilder->optType(); }
	const char* toString(){ return _sqlBuilder->toString(); }
	bool checkVaild(){ return _sqlBuilder->checkVaild(); }
	bool submit(){ return _sqlBuilder->submit(); }

	SQLCommand& table(const char* tableName){ _sqlBuilder->table(tableName); return *this;}
	SQLCommand& get(const int32 limit = 0){ _sqlBuilder->get(limit); return *this;}
	SQLCommand& del(){ _sqlBuilder->del(); return *this;}

	template<typename... Args>
	SQLCommand& select(const char* field, Args... args){
		return selectInner(field, args...);
	}
	template<typename... Args>
	SQLCommand& selectInner(const char* field, Args... args){
		_sqlBuilder->select(field);
		return selectInner(args...);
	}
	SQLCommand& selectInner(const char* field){
		_sqlBuilder->select(field);
		return *this;
	}

	template<typename... Args>
	SQLCommand& where(const Expr& expr, Args... args){
		return whereInner(expr, args...);
	}
	template<typename... Args>
	SQLCommand& whereInner(const Expr& expr, Args... args){
		_sqlBuilder->where(expr);
		return whereInner(args...);
	}
	SQLCommand& whereInner(const Expr& expr){
		_sqlBuilder->where(expr);
		return *this;
	}

	template<typename... Args>
	SQLCommand& orWhere(const Expr& expr, Args... args){
		return orWhereInner(expr, args...);
	}
	template<typename... Args>
	SQLCommand& orWhereInner(const Expr& expr, Args... args){
		_sqlBuilder->orWhere(expr);
		return orWhereInner(args...);
	}
	SQLCommand& orWhereInner(const Expr& expr){
		_sqlBuilder->orWhere(expr);
		return *this;
	}

	template<typename... Args>
	SQLCommand& insert(const SetExpr* val, Args... args){
		return insertInner(val, args...);
	}
	template<typename... Args>
	SQLCommand& insertInner(const SetExpr* val, Args... args){
		_sqlBuilder->insert(val);
		return insertInner(args...);
	}
	SQLCommand& insertInner(const SetExpr* val){
		_sqlBuilder->insert(val);
		return *this;
	}

	template<typename... Args>
	SQLCommand& update(const SetExpr* val, Args... args){
		return updateInner(val, args...);
	}
	template<typename... Args>
	SQLCommand& updateInner(const SetExpr* val, Args... args){
		_sqlBuilder->update(val);
		return updateInner(val, args...);
	}
	SQLCommand& updateInner(const SetExpr* val){
		_sqlBuilder->update(val);
		return *this;
	}

	template<typename... Args>
	SQLCommand& save(const SetExpr* val, Args... args){
		return saveInner(val, args...);
	}
	template<typename... Args>
	SQLCommand& saveInner(const SetExpr* val, Args... args){
		_sqlBuilder->save(val);
		return saveInner(val, args...);
	}
	SQLCommand& saveInner(const SetExpr* val){
		_sqlBuilder->save(val);
		return *this;
	}

private:
	ISQLBuilder* _sqlBuilder;
};

class IMysqlBase{
public:
	virtual ~IMysqlBase(){}
};

class IMysqlResult{
public:
	virtual ~IMysqlResult() {}
	virtual int32 rowCount() const = 0;
	virtual int32 columnCount() const = 0;
	virtual bool columnExist(const char* column) const = 0;
	virtual int8 getDataInt8(const int32 row, const char* column) const = 0;
	virtual int16 getDataInt16(const int32 row, const char* column) const = 0;
	virtual int32 getDataInt32(const int32 row, const char* column) const = 0;
	virtual int64 getDataInt64(const int32 row, const char* column) const = 0;
	virtual const char* getDataString(const int32 row, const char* column) const = 0;
};

class IMysqlHandler{
public:
	IMysqlHandler() :_base(nullptr){}
	virtual ~IMysqlHandler() {}

	inline void setBase(IMysqlBase* base) { _base = base; }
	inline IMysqlBase* getBase() const { return _base; }
	
	virtual bool onSuccess(sl::api::IKernel* pKernel, const int32 optType, const int32 affectedRow, IMysqlResult* result) = 0;
	virtual bool onFailed(sl::api::IKernel* pKernel, const int32 optType, const int32 errCode) = 0;
	virtual void onRelease() = 0;
private:
	IMysqlBase* _base;
};

typedef std::function<void(sl::api::IKernel* pKernel, SQLCommand& sqlCommnand)> SQLCommnandFunc;
class IMysqlMgr : public sl::api::IModule{
public:
	virtual  ~IMysqlMgr(){}

	virtual void execSql(const int64 id, IMysqlHandler* handler, const SQLCommnandFunc& f) = 0;
	virtual void execSql(const int64 id, IMysqlHandler* handler, const char* sql, const char* table, const int8 optType) = 0;
	virtual void stopSql(IMysqlHandler* handler) = 0;
};
#endif
