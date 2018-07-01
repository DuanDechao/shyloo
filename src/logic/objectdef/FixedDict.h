#ifndef SL_FIXED_DICT_H
#define SL_FIXED_DICT_H

#include <string>
#include "PyDataType.h"
#include "pyscript/map.h"
#include "pyscript/pickler.h"
#include "slbinary_stream.h"
class FixedDict : public sl::pyscript::Map
{		
	/** ���໯ ��һЩpy�������������� */
	INSTANCE_SCRIPT_HREADER(FixedDict, Map)
public:	
	static PyMappingMethods mappingMethods;
	static PySequenceMethods mappingSequenceMethods;

	FixedDict(PyDataType* dataType);
	FixedDict(PyDataType* dataType, bool isPersistentsStream);

	virtual ~FixedDict();

	PyDataType* getPyDataType(void){ return _dataType; }

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
		map����������� 
	*/
	static PyObject* mp_subscript(PyObject* self, PyObject* key);

	//static int mp_ass_subscript(PyObject* self, PyObject* key, PyObject* value);

	static int mp_length(PyObject* self);

	/** 
		��ʼ���̶��ֵ�
	*/
	void initialize(std::string strDictInitData);
	void initialize(PyObject* pyDictInitData);
	void initialize(const sl::OBStream& streamInitData, bool isPersistentsStream);

	/** 
		������ݸı� 
	*/
	//bool checkDataChanged(const char* keyName, PyObject* value, bool isDelete = false);
	
	/**
		�����ֵ����ݵ��Լ��������� 
	*/
	PyObject* update(PyObject* args);

	/** 
		��ö�������� 
	*/
	PyObject* tp_repr();
	PyObject* tp_str();

protected:
	PyFixedDictType* _dataType;
} ;
#endif // SL_FIXED_DICT_H
