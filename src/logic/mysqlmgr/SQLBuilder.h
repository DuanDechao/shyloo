#ifndef __SL_CORE_SQL_BUILDER_H__
#define __SL_CORE_SQL_BUILDER_H__
#include "IMysqlMgr.h"
class DBInterface;
class SQLBuilder : public ISQLBuilder{
public:
	SQLBuilder(DBInterface* dbInterface);
	SQLBuilder(DBInterface* dbInterface, int32 optType, const char* sql);
	~SQLBuilder();

	ISQLBuilder* table(const char* tableName);
	ISQLBuilder* select(const char* field);
	ISQLBuilder* where(const Expr& expr);
	ISQLBuilder* orWhere(const Expr& expr);
	ISQLBuilder* insert(const SetExpr* val);
	ISQLBuilder* get(const int32 limit);
	ISQLBuilder* update(const SetExpr* val);
	ISQLBuilder* save(const SetExpr* val);
	ISQLBuilder* del();

	inline int32 optType() const { return _optType; }
	inline bool isRawSql() const {return _isRawSql;}
	inline const char* toString() { SLASSERT(_finalExpr != "", "wtf");  return _finalExpr.c_str(); }
	inline bool checkVaild() { return (_optType > DB_OPT_NONE) && (_table != "" || isRawSql()); }
	bool submit();
	void escapeString(string& dest, const SetExpr* val);

private:
	DBInterface*	_dbInterface;
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
