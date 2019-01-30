#ifndef __SL_CORE_SQL_BUILDER_H__
#define __SL_CORE_SQL_BUILDER_H__
#include "IMysqlMgr.h"
class DBInterface;
class SQLBuilder : public ISQLBuilder{
public:
	static SQLBuilder* create(sl::db::ISLDBConnection* dbConnect){
		return NEW SQLBuilder(dbConnect);
	}

	static SQLBuilder* create(sl::db::ISLDBConnection* dbConnect, int32 optType, const char* sql){
		return NEW SQLBuilder(dbConnect, optType, sql);
	}

	virtual ISQLBuilder* table(const char* tableName);
	virtual ISQLBuilder* select(const char* field);
	virtual ISQLBuilder* where(const Expr& expr);
	virtual ISQLBuilder* orWhere(const Expr& expr);
	virtual ISQLBuilder* insert(const SetExpr* val);
	virtual ISQLBuilder* get(const int32 limit);
	virtual ISQLBuilder* update(const SetExpr* val);
	virtual ISQLBuilder* save(const SetExpr* val);
	virtual ISQLBuilder* del();

	virtual int32 optType() const { return _optType; }
	virtual const char* toString() { SLASSERT(_finalExpr != "", "wtf");  return _finalExpr.c_str(); }
	virtual bool submit();
	virtual void release() {DEL this;}

private:
	SQLBuilder(sl::db::ISLDBConnection* dbConnect);
	SQLBuilder(sl::db::ISLDBConnection* dbConnect, int32 optType, const char* sql);
	~SQLBuilder();
	
	void escapeString(string& dest, const SetExpr* val);
	inline bool isRawSql() const {return _isRawSql;}
	inline bool checkVaild() { return (_optType > DB_OPT_NONE) && (_table != "" || isRawSql()); }

private:
	sl::db::ISLDBConnection* _dbConnect;
	string			_table;
	int32			_optType;
	int32			_limit;
	bool			_isRawSql;

	Expr			_valuesExpr;
	Expr			_whereExpr;
	string			_selectExpr;
	string			_finalExpr;

};
#endif
