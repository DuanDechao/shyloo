#ifndef _SL_INTERFACE_SCRIPT_ENGINE_H__
#define _SL_INTERFACE_SCRIPT_ENGINE_H__
#include "slimodule.h"
#include <functional>

class IScriptTable;
class IScriptParamsReader{
public:
	virtual ~IScriptParamsReader() {}

	virtual int32 count() const = 0;
	virtual bool getBoolean(const int32 index) const = 0;
	virtual int8 getInt8(const int32 index) const = 0;
	virtual int16 getInt16(const int32 index) const = 0;
	virtual int32 getInt32(const int32 index) const = 0;
	virtual int64 getInt64(const int32 index) const = 0;
	virtual float getFloat(const int32 index) const = 0;
	virtual const char* getString(const int32 index) const = 0;
	virtual void* getPointer(const int32 index) const = 0;
	virtual IScriptTable* getTable(const int32 index) const = 0;
};

class IScriptParamsWriter{
public:
	virtual ~IScriptParamsWriter() {}

	virtual int32 count() = 0;

	virtual void addBoolean(const bool value) = 0;
	virtual void addInt8(const int8 value) = 0;
	virtual void addInt16(const int16 value) = 0;
	virtual void addInt32(const int32 value) = 0;
	virtual void addInt64(const int64 value) = 0;
	virtual void addFloat(const float value) = 0;
	virtual void addString(const char* value) = 0;
	virtual void addPointer(void* value) = 0;
};

typedef std::function<void(sl::api::IKernel* pKernel, IScriptParamsReader* reader, IScriptParamsWriter* writer)> ScriptFuncType;

class IScriptResult{
public:
	virtual ~IScriptResult(){}

	virtual int32 count()const = 0;
	virtual bool getBool(const int32 index) const = 0;
	virtual int8 getInt8(const int32 index) const = 0;
	virtual int16 getInt16(const int32 index)const = 0;
	virtual int32 getInt32(const int32 index) const = 0;
	virtual int64 getInt64(const int32 index) const = 0;
	virtual float getFloat(const int32 index) const = 0;
	virtual const char* getString(const int32 index)const = 0;
	virtual void* getPointer(const int32 index)const = 0;
	virtual IScriptTable* getTable(const int32 index)const = 0;
};

typedef std::function<void(sl::api::IKernel* pKernel, const IScriptResult* result)> ScriptResultReadFuncType;

class IScriptCallor{
public:
	virtual ~IScriptCallor(){}

	virtual void addBool(bool value) = 0;
	virtual void addInt8(int8 value) = 0;
	virtual void addInt16(int16 value) = 0;
	virtual void addInt32(int32 value) = 0;
	virtual void addInt64(int64 value) = 0;
	virtual void addFloat(float value) = 0;
	virtual void addDouble(double value) = 0;
	virtual void addString(const char* value) = 0;
	virtual void addPointer(void* value) = 0;

	virtual bool call(sl::api::IKernel* pKernel, const ScriptResultReadFuncType& f) = 0;
};


class IScriptTable{
public:
	virtual ~IScriptTable() {}

	virtual void release() = 0;

	virtual bool getBoolean(const int64 key) = 0;
	virtual int8 getInt8(const int64 key) = 0;
	virtual int16 getInt16(const int64 key) = 0;
	virtual int32 getInt32(const int64 key) = 0;
	virtual int64 getInt64(const int64 key) = 0;
	virtual float getFloat(const int64 key) = 0;
	virtual void* getPointer(const int64 key) = 0;
	virtual IScriptTable* getTable(const int64 key) = 0;
	virtual int32 getArrayCount() = 0;

	template<int16 size>
	void getString(const int32 key, string& str){
		str = getString(key);
		freeString();
	}

	virtual bool getBoolean(const char* key) = 0;
	virtual int8 getInt8(const char* key) = 0;
	virtual int16 getInt16(const char* key) = 0;
	virtual int32 getInt32(const char* key) = 0;
	virtual int64 getInt64(const char* key) = 0;
	virtual float getFloat(const char* key) = 0;
	virtual void* getPointer(const char* key) = 0;
	virtual IScriptTable* getTable(const char* key) = 0;

	template<int16 size>
	void getString(const char* key, string& str){
		str = getString(key);
		freeString();
	}

protected:
	virtual const char* getString(const int32 key) = 0;
	virtual const char* getString(const char* key) = 0;
	virtual void freeString() = 0;
};

class IScriptEngine: public sl::api::IModule{
public:
	virtual ~IScriptEngine() {}

	virtual IScriptCallor* prepareCall(const char* module, const char* func) = 0;
	virtual void RsgModuleFunc(const char* module, const char* func, const ScriptFuncType& f, const char* debug) = 0;
};

#define RGS_SCRIPT_FUNC(engine, module, func, f) engine->RsgModuleFunc(module, func, f, #f)

#endif