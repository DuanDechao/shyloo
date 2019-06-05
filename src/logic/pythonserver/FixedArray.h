#ifndef _FIXED_ARRAY_TYPE_H
#define _FIXED_ARRAY_TYPE_H
#include <string>
#include "PyDataType.h"
#include "pyscript/sequence.h"
#include "pyscript/pickler.h"
class FixedArray: public sl::pyscript::ScriptObject{		
	/** 子类化 将一些py操作填充进派生类 */
	INSTANCE_SCRIPT_HREADER(FixedArray, ScriptObject)
public:	
	static PySequenceMethods seqMethods;

	FixedArray(PyDataType* dataType, IArray* objArray = NULL);
	~FixedArray();

	inline PyDataType* getDataType(void){ return _dataType; }
	inline IDataType* getSubDataType() {return _dataType->arrayDataType();}
	inline int length(void) const{ return _objArray ? (int)_objArray->length() : 0; } 
	inline IArray* objArray() {return _objArray;}
	PyObject* getDataByIndex(const int32 index);
	void setDataByIndex(const int32 index, PyObject* pyobj);
	bool removeDataByIndex(const int32 start, const int32 end);
	bool extendByIndex(const int32 index, const int32 count);
	bool repeat(const int32 n);
	bool clear();
	
	/** 
		初始化固定数组
	*/
	void initialize(std::string strInitData);
	void initialize(PyObject* pyObjInitData);

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

	int findFrom(uint32 startIndex, PyObject* value);
	
	/** 
		一下为一个list所需的操作接口 
	*/
	static PyObject* __py_append(PyObject* self, PyObject* args, PyObject* kwargs);	
	static PyObject* __py_count(PyObject* self, PyObject* args, PyObject* kwargs);
	static PyObject* __py_extend(PyObject* self, PyObject* args, PyObject* kwargs);	
	static PyObject* __py_index(PyObject* self, PyObject* args, PyObject* kwargs);
	static PyObject* __py_insert(PyObject* self, PyObject* args, PyObject* kwargs);	
	static PyObject* __py_pop(PyObject* self, PyObject* args, PyObject* kwargs);
	static PyObject* __py_remove(PyObject* self, PyObject* args, PyObject* kwargs);

	static Py_ssize_t seq_length(PyObject* self);
	static PyObject* seq_concat(PyObject* self, PyObject* seq);
	static PyObject* seq_repeat(PyObject* self, Py_ssize_t n);
	static PyObject* seq_item(PyObject* self, Py_ssize_t index);
	static PyObject* seq_slice(PyObject* self, Py_ssize_t startIndex, Py_ssize_t endIndex);
	static int seq_ass_item(PyObject* self, Py_ssize_t index, PyObject* value);
	static int seq_ass_slice(PyObject* self, Py_ssize_t index1, Py_ssize_t index2, PyObject* oterSeq);
	static int seq_contains(PyObject* self, PyObject* value);
	static PyObject* seq_inplace_concat(PyObject* self, PyObject* oterSeq);
	static PyObject* seq_inplace_repeat(PyObject * self, Py_ssize_t n);
	
	bool isSameType(PyObject* pyValue);
	bool isSameItemType(PyObject* pyValue);

	PyObject* createNewItemFromObj(PyObject* pyItem);

	/** 
		获得对象的描述 
	*/
	PyObject* tp_repr();
	PyObject* tp_str();


protected:
	PyFixedArrayType* _dataType;
	IArray*			  _objArray;
} ;
#endif
