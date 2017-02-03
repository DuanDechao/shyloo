#ifndef __SL_PUBLIC_CALL_BACK_H__
#define __SL_PUBLIC_CALL_BACK_H__
#include "slmulti_sys.h"
#include <list>
#include <functional>
#include <unordered_map>
namespace sl{

template<typename T, const int32 debugSize = 256>
struct Caller{
	T _call;
	char _info[debugSize];

	Caller(const T& val, const char* info){
		_call = val;
		SafeSprintf(_info, sizeof(_info), "%s", info);
	}

	Caller& operator=(const Caller& caller){
		_call = caller._call;
		SafeSprintf(_info, sizeof(_info), "%s", caller._info);
		return *this;
	}

	bool operator==(const Caller& caller) const {
		return strcmp(_info, caller._info) == 0;
	}
};

template<typename T, typename R, typename... Args>
class CallBack{
	typedef std::function<R(Args...)> FuncType;
	typedef std::list<Caller<FuncType>> FuncList;

public:
	CallBack(){}
	~CallBack(){}

	void Register(const T id, const FuncType& f, const char* info){
		auto itor = _funcs.find(id);
		if (itor == _funcs.end()){
			_funcs[id];
			itor = _funcs.find(id);
		}

		SLASSERT(itor != _funcs.end(), "wtdf");

#ifdef _DEBUG
		auto funcItor = std::find(itor->second.begin(), itor->second.end(), Caller<FuncType>(f, info));
		SLASSERT(funcItor == itor->second.end(), "function has exist");
#endif

		itor->second.push_back(Caller<FuncType>(f, info));
		
	}

	void UnRegister(const T id, const FuncType& f, const char* info){
		auto itor = _funcs.find(id);
		if (itor != _funcs.end()){
			std::move(itor->second.begin(), itor->second.end(), Caller<FuncType>(f, info));
		}
	}

	void UnRegister(const T id){
		_funcs.erase(id);
	}

	inline void Clear() { _funcs.clear(); }

	void Call(const T id, Args... a){
		auto itor = _funcs.find(id);
		if (itor != _funcs.end()){
			for (const auto& cb : itor->second){
				cb._call(a...);
			}
		}
	}

	R Call(const T id, const R& r, Args... a){
		auto itor = _funcs.find(id);
		if (itor != _funcs.end()){
			for (const auto& cb : itor->second){
				if (r == cb._call(a...)){
					return r;
				}
			}
		}
		return r;
	}
private:
	std::unordered_map<T, FuncList> _funcs;
};

template<typename T, typename... Args>
class CallBack<T, void, Args...>{
	typedef std::function<void(Args...)> FuncType;
	typedef std::list<Caller<FuncType>> FuncList;

public:
	CallBack(){}
	~CallBack(){}

	void Register(const T id, const FuncType& f, const char* info){
		auto itor = _funcs.find(id);
		if (itor == _funcs.end()){
			_funcs[id];
			itor = _funcs.find(id);
		}

		SLASSERT(itor != _funcs.end(), "wtdf");

#ifdef _DEBUG
		auto funcItor = std::find(itor->second.begin(), itor->second.end(), Caller<FuncType>(f, info));
		SLASSERT(funcItor == itor->second.end(), "function has exist");
#endif

		itor->second.push_back(Caller<FuncType>(f, info));
	}

	void UnRegister(const T id, const FuncType& f, const char* info){
		auto itor = _funcs.find(id);
		if (itor != _funcs.end()){
			std::move(itor->second.begin(), itor->second.end(), Caller<FuncType>(f, info));
		}
	}

	inline void UnRegister(const T id) { _funcs.erase(id); }
	inline void Clear(){ _funcs.clear(); }

	void Call(const T id, Args... a){
		auto itor = _funcs.find(id);
		if (itor != _funcs.end()){
			for (const auto& cb: itor->second){
				cb._call(a...);
			}
		}
	}

private:
	std::unordered_map<T, FuncList> _funcs; 
};

template<typename T, typename C>
struct CallBackType{};


template<typename T, typename R, typename... Args>
struct CallBackType<T, std::function<R(Args...)>> {
	typedef CallBack<T, R, Args...> type;
};

}
#endif