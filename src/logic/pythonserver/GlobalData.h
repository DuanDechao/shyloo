#ifndef SL_LOGIC_GLOBAL_DATA_H
#define SL_LOGIC_GLOBAL_DATA_H
// common include	
#include "pyscript/scriptobject.h"
class GlobalData : public sl::pyscript::ScriptObject{
	BASE_SCRIPT_HREADER(GlobalData, ScriptObject)	
public:
	GlobalData():
		ScriptObject(getScriptType(), false),
		_dataDict(PyDict_New())
	{}

	~GlobalData(){
		Py_DECREF(_dataDict);
	}	

	/** 
		暴露一些字典方法给python 
	*/
	DECLARE_PY_MOTHOD_ARG1(pyHas_key, const_charptr);
	DECLARE_PY_MOTHOD_ARG0(pyKeys);
	
	static PyObject* __py_pyGet(PyObject * self, 
		PyObject * args, PyObject* kwds);

	/** 
		map操作函数相关 
	*/
	static PyObject* mp_subscript(PyObject * self, PyObject * key);
	static int mp_length(PyObject * self);
	static int mp_ass_subscript(PyObject* self, PyObject* key, PyObject* value);

	static PyMappingMethods mappingMethods;
	static PySequenceMethods mappingSequenceMethods;
private:
	PyObject* _dataDict;
};
#endif
