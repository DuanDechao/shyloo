#include "SQLBuilder.h"
#include "MysqlMgr.h"
#include "DBInterface.h"
SQLBuilder::SQLBuilder(DBInterface* dbInterface)
	:_table(""),
	 _optType(DB_OPT_NONE),
	 _limit(0),
	 _selectExpr("*"),
	 _finalExpr(""),
	 _dbInterface(dbInterface),
	 _isRawSql(false)
{}

SQLBuilder::SQLBuilder(DBInterface* dbInterface, int32 optType, const char* sql)
	:_table(""),
	_optType(optType),
	_limit(0),
	_selectExpr("*"),
	_finalExpr(string(sql)),
	_dbInterface(dbInterface),
	_isRawSql(true)
{}

SQLBuilder::~SQLBuilder(){
	for (auto& wWxpr : _whereExpr.exprs()){
		if (wWxpr)
			DEL wWxpr;
	}

	for (auto& vExpr : _valuesExpr.exprs()){
		if (vExpr)
			DEL vExpr;
	}

	_whereExpr.exprs().clear();
	_valuesExpr.exprs().clear();
}

ISQLBuilder* SQLBuilder::table(const char* table){
	SLASSERT(table, "wtf");
	_table.append("`");
	_table.append(table);
	_table.append("`");
	return this;
}

ISQLBuilder* SQLBuilder::select(const char* field){
	SLASSERT(field, "wtf");
	string fieldStr = "`";
	fieldStr += field;
	fieldStr += "`";

	if (_selectExpr == "*")
		_selectExpr = fieldStr;
	else
		_selectExpr += ", " + fieldStr;

	return this;
}

ISQLBuilder* SQLBuilder::where(const Expr& expr){
	_whereExpr = std::move(_whereExpr && expr);
	return this;
}

ISQLBuilder* SQLBuilder::orWhere(const Expr& expr){
	_whereExpr = std::move(_whereExpr || expr);
	return this;
}

ISQLBuilder* SQLBuilder::insert(const SetExpr* val){
	if (_optType != DB_OPT_NONE && _optType != DB_OPT_INSERT){
		SLASSERT(false, "invaild sql opt");
		return this;
	}

	_optType = DB_OPT_INSERT;
	_valuesExpr.append(val);
	return this;
}

ISQLBuilder* SQLBuilder::get(const int32 limit){
	if (_optType != DB_OPT_NONE && _optType != DB_OPT_QUERY){
		SLASSERT(false, "invaild sql opt");
		return this;
	}
	
	_optType = DB_OPT_QUERY;
	_limit = limit;
	return this;
}

ISQLBuilder* SQLBuilder::update(const SetExpr* val){
	if (_optType != DB_OPT_NONE && _optType != DB_OPT_UPDATE){
		SLASSERT(false, "invaild sql opt");
		return this;
	}

	_optType = DB_OPT_UPDATE;
	_valuesExpr.append(val);
	return this;
}

ISQLBuilder* SQLBuilder::save(const SetExpr* val){
	if (_optType != DB_OPT_NONE && _optType != DB_OPT_SAVE){
		SLASSERT(false, "invaild sql opt");
		return this;
	}

	_optType = DB_OPT_SAVE;
	_valuesExpr.append(val);
	return this;
}

ISQLBuilder* SQLBuilder::del(){
	if (_optType != DB_OPT_NONE && _optType != DB_OPT_DELETE){
		SLASSERT(false, "invaild sql opt");
		return this;
	}

	_optType = DB_OPT_DELETE;
	return this;
}

bool SQLBuilder::submit(){
	if (!checkVaild()){
		SLASSERT(false, "invaild sql command");
		return false;
	}
	
	if (_finalExpr != "")
		return true;
	
	switch (_optType){
		case DB_OPT_INSERT:{
			if (_optType != DB_OPT_INSERT || _valuesExpr.exprs().empty()){
				SLASSERT(false, "sql command is not insert or have no value ");
				return false;
			}
			
			_finalExpr += "INSERT " + _table + " (";
			
			int32 i = 0;
			for (auto& val : _valuesExpr.exprs()){
				if (i != 0)
					_finalExpr += ",";
				_finalExpr += val->field();
				i++;
			}

			_finalExpr += ") VALUES (";

			i = 0;
			for (auto& val : _valuesExpr.exprs()){
				if (i != 0)
					_finalExpr += ",";
				escapeString(_finalExpr, val);
				i++;
			}
			_finalExpr += ")";

			break;
		}
		case DB_OPT_QUERY:{
			if (_optType != DB_OPT_QUERY){
				SLASSERT(false,  "sql command is not query");
				return false;
			}

			_finalExpr += "SELECT " + _selectExpr + " FROM " + _table;
			if (!_whereExpr.exprs().empty()){
				_finalExpr += " WHERE ";
				for (auto& expr : _whereExpr.exprs()){
					_finalExpr += expr->field();
					escapeString(_finalExpr, expr);
				}
			}

			break;
		}
		case DB_OPT_UPDATE:{
			if (_optType != DB_OPT_UPDATE || _valuesExpr.exprs().empty()){
				SLASSERT(false, "sql command is not update or have no value");
				return false;
			}

			_finalExpr += "UPDATE " + _table + " SET ";
			int32 i = 0;
			for (auto& val : _valuesExpr.exprs()){
				if (i != 0)
					_finalExpr += ",";
				_finalExpr += val->field() + "=";
				escapeString(_finalExpr, val);
				i++;
			}

			if (!_whereExpr.exprs().empty()){
				_finalExpr += " WHERE ";
				for (auto& expr : _whereExpr.exprs()){
					_finalExpr += expr->field();
					escapeString(_finalExpr, expr);
				}
			}

			break;
		}
		case DB_OPT_SAVE:{
			if (_optType != DB_OPT_SAVE || _valuesExpr.exprs().empty()){
				SLASSERT(false, "sql command is not update or have no value");
				return false;
			}

			_finalExpr += "INSERT INTO " + _table + "(";
			int32 i = 0;
			for (auto& val : _valuesExpr.exprs()){
				if (i != 0)
					_finalExpr += ",";
				_finalExpr += val->field();
				i++;
			}

			_finalExpr += ") VALUES (";
			i = 0;
			for (auto& val : _valuesExpr.exprs()){
				if (i != 0)
					_finalExpr += ",";
				escapeString(_finalExpr, val);
				i++;
			}

			_finalExpr += ")ON DUPLICATE KEY UPDATE ";
			i = 0;
			for (auto& val : _valuesExpr.exprs()){
				if (i != 0)
					_finalExpr += ",";
				_finalExpr += val->field();
				_finalExpr += "=";
				escapeString(_finalExpr, val);
				i++;
			}

			break;
		}
		case DB_OPT_DELETE:{
			if (_optType != DB_OPT_DELETE){
				SLASSERT(false, "sql command is not delete");
				return false;
			}

			_finalExpr += "DELETE FROM " + _table;
			if (!_whereExpr.exprs().empty()){
				_finalExpr += " WHERE ";
				for (auto& expr : _whereExpr.exprs()){
					_finalExpr += expr->field();
					escapeString(_finalExpr, expr);
				}
			}

			break;
		}
		default:{
			SLASSERT(false, "unknown sql opt");
			return false;
		}
	}
	return true;
}

void SQLBuilder::escapeString(string& dest, const SetExpr* val){
	if (!val->needEsacape()){
		dest += val->value();
	}else{
		int32 srcSize = (int32)(val->value().size());
		char* destStr = (char*)alloca(srcSize * 2 + 3);
		int32 escapeLen = _dbInterface->escapeString(destStr, srcSize * 2 + 3, val->value().c_str(), srcSize);
		dest.append("'");
		dest.append(destStr, escapeLen);
		dest.append("'");
	}
}

