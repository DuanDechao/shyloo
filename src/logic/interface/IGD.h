#ifndef __SL_INTERFACE_IGD_H__
#define __SL_INTERFACE_IGD_H__
#include "slimodule.h"
#include "slikernel.h"
#include "sltools.h"
class IObject;
class IGDCommandHandler{
public:
	virtual ~IGDCommandHandler() {}
	virtual int32 dealCommand(sl::api::IKernel* pKernel, IObject* player, char* args) = 0;
};

namespace gd{
	enum{
		GD_NONE = -3,
		GD_ERROR = -2,
		GD_ARGS_ERROR = -1,
		GD_SUCCESS = 0,
	};

	typedef char* StringType;
	typedef const char* ConstStringType;

	inline void Next(char* param, StringType& next){
		next = strstr(param, " ");
		if (next){
			*next = 0;
			++next;
		}
	}

	inline bool Parse(char* param, int8& t, StringType& next){
		if (param){
			Next(param, next);
			t = (int8)sl::CStringUtils::StringAsInt8(param);
			return true;
		}
		return false;
	}

	inline bool Parse(char* param, int16& t, StringType& next){
		if (param){
			Next(param, next);
			t = (int16)sl::CStringUtils::StringAsInt16(param);
			return true;
		}
		return false;
	}

	inline bool Parse(char* param, int32& t, StringType& next){
		if (param){
			Next(param, next);
			t = (int32)sl::CStringUtils::StringAsInt32(param);
			return true;
		}
		return false;
	}

	inline bool Parse(char* param, int64& t, StringType& next){
		if (param){
			Next(param, next);
			t = (int64)sl::CStringUtils::StringAsInt64(param);
			return true;
		}
		return false;
	}

	inline bool Parse(char* param, float& t, StringType& next){
		if (param){
			Next(param, next);
			t = sl::CStringUtils::StringAsFloat(param);
			return true;
		}
		return false;
	}

	inline bool Parse(char* param, ConstStringType& t, StringType& next){
		if (param){
			Next(param, next);
			t = param;
			return true;
		}
		return false;
	}

template<typename... FixArgs>
struct Handler{
	template<typename... ParsedArgs>
	struct Dealer{
		static int32 invoke(FixArgs... first, char * param, int32(*fn)(FixArgs..., ParsedArgs...), ParsedArgs... args){
			return fn(first..., args...);
		}
		template<typename T, typename... Args>
		static int32 invoke(FixArgs... first, char* param, int32(*fn)(FixArgs..., ParsedArgs..., T, Args...), ParsedArgs... args){
			char *next = nullptr;
			T t;
			if (!Parse(param, t, next))
				return GD_ARGS_ERROR;
			return Dealer<ParsedArgs..., T>::invoke(first..., next, fn, args..., t);
		}
	};

	template<typename... Args>
	static int32 deal(FixArgs... first, char* param, int32(*fn)(FixArgs..., Args...)){
		return Dealer<>::invoke(first..., param, fn);
	}

	static int32 deal(FixArgs... first, char* param, int32(*fn)(FixArgs...)){
		return fn(first...);
	}
};

template<typename... Args>
class GDCommandHandler : public IGDCommandHandler{
	typedef int32(*call_func)(sl::api::IKernel*, IObject*, Args...);

public:
	GDCommandHandler(call_func f) :_f(f){}
	virtual ~GDCommandHandler() {}
	
	virtual int32 dealCommand(sl::api::IKernel* pKernel, IObject* player, char* args){
		return Handler<sl::api::IKernel*, IObject*>::deal(pKernel, player, args, _f);
	}

private:
	call_func _f;
};
}

class IGD : public sl::api::IModule{
	
public:
	virtual ~IGD() {}
	template<typename... Args>
	void rsgGD(const char* command, int32(*fn)(sl::api::IKernel*, IObject*, Args...)){
		rsgGDInner(command, NEW gd::GDCommandHandler(fn));
	}

protected:
	virtual void rsgGDInner(const char* command, IGDCommandHandler* handler) = 0;
};


#endif