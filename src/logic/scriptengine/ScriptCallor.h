#ifndef __SL_CORE_SCRIPT_CALLOR_H__
#define __SL_CORE_SCRIPT_CALLOR_H__
#include "slmulti_sys.h"
#include "IScriptEngine.h"

class ScriptEngine;
class ScriptCallor: public IScriptCallor{
public:
	ScriptCallor(ScriptEngine* engine) : _engine(engine), _using(false), _count(0), _top(0){}

	virtual void addBool(bool value);
	virtual void addInt8(int8 value);
	virtual void addInt16(int16 value);
	virtual void addInt32(int32 value);
	virtual void addInt64(int64 value);
	virtual void addFloat(float value);
	virtual void addDouble(double value);
	virtual void addString(const char* value);
	virtual void addPointer(void* value);

	virtual void use(const int32 top){ _using = true; _top = top; _count = 0; }
	virtual bool isUsing(){ return _using;}
	virtual bool call(sl::api::IKernel* pKernel, const ScriptResultReadFuncType& f);

private:
	ScriptEngine* _engine;
	bool		  _using;
	int32		  _count;
	int32		  _top;
};
#endif
