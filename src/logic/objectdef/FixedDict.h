#ifndef SL_FIXED_DICT_H
#define SL_FIXED_DICT_H

#include <string>
#include "PyDataType.h"
#include "pyscript/map.h"
#include "pyscript/pickler.h"
#include "slbinary_stream.h"
class FixedDict : public sl::pyscript::Map
{		
	/** 子类化 将一些py操作填充进派生类 */
	INSTANCE_SCRIPT_HREADER(FixedDict, Map)
public:	
	static PyMappingMethods mappingMethods;
	static PySequenceMethods mappingSequenceMethods;

	FixedDict(PyDataType* dataType);
	FixedDict(PyDataType* dataType, bool isPersistentsStream);

	virtual ~FixedDict();

	PyDataType* getPyDataType(void){ return _dataType; }

	/** 
		支持pickler 方法 
	*/
	static PyObject* __py_reduce_ex__(PyObject* self, PyObject* protocol);

	/** 
		unpickle方法 
	*/
	static PyObject* __unpickle__(PyObject* self, PyObject* args);
	
	/** 
		脚本被安装时被调用 
	*/
	static void onInstallScript(PyObject* mod);
	
	/** 
		map操作函数相关 
	*/
	static PyObject* mp_subscript(PyObject* self, PyObject* key);

	//static int mp_ass_subscript(PyObject* self, PyObject* key, PyObject* value);

	static int mp_length(PyObject* self);

	/** 
		初始化固定字典
	*/
	void initialize(std::string strDictInitData);
	void initialize(PyObject* pyDictInitData);
	void initialize(const sl::OBStream& streamInitData, bool isPersistentsStream);

	/** 
		检查数据改变 
	*/
	//bool checkDataChanged(const char* keyName, PyObject* value, bool isDelete = false);
	
	/**
		更新字典数据到自己的数据中 
	*/
	PyObject* update(PyObject* args);

	/** 
		获得对象的描述 
	*/
	PyObject* tp_repr();
	PyObject* tp_str();

protected:
	PyFixedDictType* _dataType;
} ;
#endif // SL_FIXED_DICT_H
