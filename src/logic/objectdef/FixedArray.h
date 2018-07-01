#ifndef _FIXED_ARRAY_TYPE_H
#define _FIXED_ARRAY_TYPE_H
#include <string>
#include "PyDataType.h"
#include "pyscript/sequence.h"
#include "pyscript/pickler.h"
class FixedArray : public sl::pyscript::Sequence
{		
	/** ���໯ ��һЩpy�������������� */
	INSTANCE_SCRIPT_HREADER(FixedArray, Sequence)
public:	
	FixedArray(PyDataType* dataType);
	virtual ~FixedArray();

	const PyDataType* getDataType(void){ return _dataType; }
	
	/** 
		��ʼ���̶�����
	*/
	void initialize(std::string strInitData);
	void initialize(PyObject* pyObjInitData);

	/** 
		֧��pickler ���� 
	*/
	static PyObject* __py_reduce_ex__(PyObject* self, PyObject* protocol);

	/** 
		unpickle���� 
	*/
	static PyObject* __unpickle__(PyObject* self, PyObject* args);
	
	/** 
		�ű�����װʱ������ 
	*/
	static void onInstallScript(PyObject* mod);
	
	/** 
		һ��Ϊһ��list����Ĳ����ӿ� 
	*/
	static PyObject* __py_append(PyObject* self, PyObject* args, PyObject* kwargs);	
	static PyObject* __py_count(PyObject* self, PyObject* args, PyObject* kwargs);
	static PyObject* __py_extend(PyObject* self, PyObject* args, PyObject* kwargs);	
	static PyObject* __py_index(PyObject* self, PyObject* args, PyObject* kwargs);
	static PyObject* __py_insert(PyObject* self, PyObject* args, PyObject* kwargs);	
	static PyObject* __py_pop(PyObject* self, PyObject* args, PyObject* kwargs);
	static PyObject* __py_remove(PyObject* self, PyObject* args, PyObject* kwargs);
	
	bool isSameType(PyObject* pyValue);
	bool isSameItemType(PyObject* pyValue);

	virtual PyObject* createNewItemFromObj(PyObject* pyItem);

	/** 
		��ö�������� 
	*/
	PyObject* tp_repr();
	PyObject* tp_str();
protected:
	PyFixedArrayType* _dataType;
} ;
#endif
