#ifndef __SL_CORE_SQL_BASE_H__
#define __SL_CORE_SQL_BASE_H__
#include <sstream>
#include <list>
enum DB_OPT{
	DB_OPT_NONE = 0,
	DB_OPT_QUERY,
	DB_OPT_INSERT,
	DB_OPT_UPDATE,
	DB_OPT_SAVE,
	DB_OPT_DELETE,
};

struct SerializeHelper{
	template<class T>
	inline static bool Serialize(std::ostream& os, const T& val){
		os << std::move(val);
		return false;
	}

	inline static bool Serialize(std::ostream& os, const int8 val){
		os << (int)val;
		return false;
	}

	inline static bool Serialize(std::ostream& os, const uint8 val){
		os << (int)val;
		return false;
	}

	inline static bool Serialize(std::ostream& os, const char* val){
		os << val;
		return true;
	}

	inline static bool Serialize(std::ostream& os, const string& val){
		os << val;
		return true;
	}
};

struct SetExpr{
	SetExpr(const string& field, const void* value, const int32 size, bool needEscape){
		_field = std::move(field);
		_value.append((const char*)value, size);
		_needEscape = needEscape;
	}

	inline const string& field() const { return _field; }
	inline const string& value() const { return _value; }
	inline bool needEsacape() const { return _needEscape; }

private:
	string _field;
	string _value;
	bool _needEscape;
};

struct Field{
	Field(const char* fieldName){
		_field.append("`");
		_field.append(fieldName);
		_field.append("`");
	}

	template<class T>
	SetExpr* operator = (const T value){
		ostringstream os;
		bool bEscape = SerializeHelper::Serialize(os, value);
		return NEW SetExpr(_field, os.str().c_str(), (int32)(os.str().size()), bEscape);
	}

	SetExpr* addStruct(const void* val, const int32 size){
		return NEW SetExpr(_field, val, size, true);
	}

	string _field;
};

struct Expr{
	Expr(){}
	template<class T>
	Expr(const Field& field, const string& op, T& value){
		std::ostringstream os;
		bool bEscape = SerializeHelper::Serialize(os, value);
		string valStr = os.str();
		_exprs.push_back(NEW SetExpr(field._field + op, valStr.c_str(), (int32)valStr.size(), bEscape));
	}

	void append(const SetExpr* expr){
		_exprs.push_back(expr);
	}
	void append(const Expr& expr){
		_exprs.splice(_exprs.cend(), std::move(expr.exprs()));
	}

	inline Expr operator && (const Expr& expr){
		return and_or(expr, _exprs.empty() ? "" : " and ");
	}
	inline Expr operator || (const Expr& expr){
		return and_or(expr, _exprs.empty() ? "" : " or ");
	}

	inline std::list<const SetExpr*> exprs() const { return _exprs; }

private:
	std::list<const SetExpr*> _exprs;

	Expr and_or(const Expr& right,	const std::string& logOp) const{
		auto ret = *this;
		auto rigthExprs = right.exprs();
		ret._exprs.push_front(NEW SetExpr("(", nullptr, 0, false));
		ret._exprs.push_back(NEW SetExpr(std::move(logOp), nullptr, 0, false));
		ret._exprs.splice(ret._exprs.cend(), std::move(rigthExprs));
		ret._exprs.push_back(NEW SetExpr(")", nullptr, 0, false));
		return ret;
	}
};


template<class T>
Expr operator == (const Field& field, const T val){
	return Expr(field, "=", std::move(val));
}

template<class T>
Expr operator != (const Field& field, const T val){
	return Expr(field, "!=", std::move(val));
}

template<class T>
Expr operator > (const Field& field, const T val){
	return Expr(field, ">", std::move(val));
}

template<class T>
Expr operator < (const Field& field, const T val){
	return Expr(field, "<", std::move(val));
}

template<class T>
Expr operator >= (const Field& field, const T val){
	return Expr(field, ">=", std::move(val));
}

template<class T>
Expr operator <= (const Field& field, const T val){
	return Expr(field, "<=", std::move(val));
}
#endif
