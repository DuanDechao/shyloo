#include "GlobalData.h"
#include "slstring_utils.h"
#include "IGlobalData.h"
#include "IPythonEngine.h"
PyMappingMethods GlobalData::mappingMethods ={
	(lenfunc)mp_length,								// mp_length
	(binaryfunc)mp_subscript,						// mp_subscript
	(objobjargproc)mp_ass_subscript					// mp_ass_subscript
};

// ²Î¿¼ objects/dictobject.c
// Hack to implement "key in dict"
PySequenceMethods GlobalData::mappingSequenceMethods = 
{
    0,											/* sq_length */
    0,											/* sq_concat */
    0,											/* sq_repeat */
    0,											/* sq_item */
    0,											/* sq_slice */
    0,											/* sq_ass_item */
    0,											/* sq_ass_slice */
    PyMapping_HasKey,							/* sq_contains */
    0,											/* sq_inplace_concat */
    0,											/* sq_inplace_repeat */
};

SCRIPT_METHOD_DECLARE_BEGIN(GlobalData)
SCRIPT_METHOD_DECLARE("has_key",			pyHas_key,				METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("keys",				pyKeys,					METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("get",				pyGet,					METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_MEMBER_DECLARE_BEGIN(GlobalData)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_INIT(GlobalData, 0, &GlobalData::mappingSequenceMethods, &GlobalData::mappingMethods, 0, 0)	

//-------------------------------------------------------------------------------------
int GlobalData::mp_length(PyObject * self){
	return PyDict_Size(static_cast<GlobalData*>(self)->_dataDict);
}
	
//-------------------------------------------------------------------------------------
PyObject* GlobalData::mp_subscript(PyObject* self, PyObject* key /*entityID*/){
	PyObject* pyObj = PyDict_GetItem(static_cast<GlobalData*>(self)->_dataDict, key);
	if(pyObj){
		Py_INCREF(pyObj);
		return pyObj;
	}

	wchar_t* pyUnicodeWideString = PyUnicode_AsWideCharString(key, NULL);
	char* keyStr = sl::CStringUtils::wchar2char(pyUnicodeWideString);
	PyMem_Free(pyUnicodeWideString);
	if (PyErr_Occurred())
		return NULL;
	
	int16 dataType = 0;
	int32 dataSize = 0;
	const void* data = SLMODULE(GlobalDataClient)->getData(keyStr, dataType, dataSize);
	if(data && dataSize > 0){
		std::string datas((const char*)data, dataSize);
		pyObj = SLMODULE(PythonEngine)->unpickle(datas);
	}

	free(keyStr);

	if(!pyObj)
		PyErr_SetObject(PyExc_KeyError, key);
	else
		Py_INCREF(pyObj);

	PyDict_SetItem(static_cast<GlobalData*>(self)->_dataDict, key, pyObj);
	return pyObj;
}

int GlobalData::mp_ass_subscript(PyObject* self, PyObject* key, PyObject* value){
	wchar_t* pyUnicodeWideString = PyUnicode_AsWideCharString(key, NULL);
	char* keyStr = sl::CStringUtils::wchar2char(pyUnicodeWideString);
	PyMem_Free(pyUnicodeWideString);
	if (PyErr_Occurred())
		return NULL;
	
	if(value == NULL){
		SLMODULE(GlobalDataClient)->del(keyStr);
		return PyDict_DelItem(static_cast<GlobalData*>(self)->_dataDict, key);
	}

	std::string val = SLMODULE(PythonEngine)->pickle(value);
	SLMODULE(GlobalDataClient)->write(keyStr, 0, val.data(), val.size());

	free(keyStr);
	return PyDict_SetItem(static_cast<GlobalData*>(self)->_dataDict, key, value);
}

//-------------------------------------------------------------------------------------

PyObject* GlobalData::pyHas_key(const char* key){
	if(!key){
		return NULL;
	}

	if(SLMODULE(GlobalDataClient)->hasData(key)){
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

//-------------------------------------------------------------------------------------

PyObject* GlobalData::pyKeys(){
	std::set<std::string>& keys = SLMODULE(GlobalDataClient)->getKeys();
	int32 keyCount = keys.size();
	PyObject* pyList = PyList_New(keyCount);
	
	int32 i = 0;
	auto itor = keys.begin();
	auto itorEnd = keys.end();
	while(itor != itorEnd){
		PyObject* keyObj = PyUnicode_FromString((*itor).c_str());
		PyList_SET_ITEM(pyList, i, keyObj);
		i++;
		++itor;
	}
	return pyList;
}

//-------------------------------------------------------------------------------------

PyObject* GlobalData::__py_pyGet(PyObject* self, PyObject * args, PyObject* kwds){
	PyObject* pDefault = Py_None;
	PyObject* key = Py_None;
	if (!PyArg_ParseTuple(args, "O|O", &key, &pDefault)){
		return NULL;
	}

	PyObject* value = mp_subscript(self, key);

	if (!value){
		value = pDefault;
		Py_INCREF(value);
	}
	return value;
}
