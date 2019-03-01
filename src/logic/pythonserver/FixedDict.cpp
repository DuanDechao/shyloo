#include "FixedDict.h"
#include "pyscript/py_gc.h"
#include "slstring_utils.h"
#include "IObjectDef.h"
#include "IPythonEngine.h"
#include "PyDataType.h"
/** python map操作所需要的方法表 */
PyMappingMethods FixedDict::mappingMethods =
{
	(lenfunc)FixedDict::mp_length,					// mp_length
	(binaryfunc)FixedDict::mp_subscript,			// mp_subscript
	(objobjargproc)FixedDict::mp_ass_subscript		// mp_ass_subscript
};

// 参考 objects/dictobject.c
// Hack to implement "key in dict"
PySequenceMethods FixedDict::mappingSequenceMethods = 
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

SCRIPT_METHOD_DECLARE_BEGIN(FixedDict)
SCRIPT_METHOD_DECLARE("__reduce_ex__",				reduce_ex__,			METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("has_key",					has_key,				METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("keys",						keys,					METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("values",						values,					METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("items",						items,					METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE_END()


SCRIPT_MEMBER_DECLARE_BEGIN(FixedDict)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(FixedDict)
SCRIPT_GETSET_DECLARE_END()
SCRIPT_INIT(FixedDict, 0, &FixedDict::mappingSequenceMethods, &FixedDict::mappingMethods, 0, 0)	
	
//-------------------------------------------------------------------------------------
FixedDict::FixedDict(PyDataType* dataType):
Map(getScriptType(), false)
{
	_dataType = static_cast<PyFixedDictType*>(dataType);
	//_dataType->incRef();

	SLMODULE(PythonEngine)->incTracing("FixedDict");

//	DEBUG_MSG(fmt::format("FixedDict::FixedDict(1): {:p}---{}\n", (void*)this,
//		wchar2char(PyUnicode_AsWideCharString(PyObject_Str(getDictObject()), NULL))));
}

//-------------------------------------------------------------------------------------
FixedDict::FixedDict(PyDataType* dataType, bool isPersistentsStream):
Map(getScriptType(), false)
{
	_dataType = static_cast<PyFixedDictType*>(dataType);
	//_dataType->incRef();
	
	SLMODULE(PythonEngine)->incTracing("FixedDict");

//	DEBUG_MSG(fmt::format("FixedDict::FixedDict(2): {:p}---{}\n", (void*)this,
//		wchar2char(PyUnicode_AsWideCharString(PyObject_Str(getDictObject()), NULL))));
}


//-------------------------------------------------------------------------------------
FixedDict::~FixedDict()
{
	//_dataType->decRef();
	SLMODULE(PythonEngine)->decTracing("FixedDict");

//	DEBUG_MSG(fmt::format("FixedDict::~FixedDict(): {:p}\n", (void*)this));
}

//-------------------------------------------------------------------------------------
/*
void FixedDict::initialize(std::string strDictInitData)
{
	FixedDictType::FIXEDDICT_KEYTYPE_MAP& keyTypes = _dataType->getKeyTypes();
	FixedDictType::FIXEDDICT_KEYTYPE_MAP::iterator iter = keyTypes.begin();
	for(; iter != keyTypes.end(); ++iter)
	{
		PyObject* pyobj = iter->second->dataType->parseDefaultStr("");
		if(pyobj)
		{
			PyObject* pykey = PyUnicode_FromString(iter->first.c_str());
			PyDict_SetItem(pyDict_, pykey, pyobj);
			Py_DECREF(pykey);
			Py_DECREF(pyobj);
		}
		else
		{
			ECHO_ERROR(fmt::format("FixedDict::initialize: error! strDictInitData={}.\n",
				strDictInitData.c_str()));
		}
	}
}
*/
//-------------------------------------------------------------------------------------

void FixedDict::initialize(PyObject* pyDictInitData)
{
	if(pyDictInitData)
	{
		update(pyDictInitData);
	}
}

//-------------------------------------------------------------------------------------
void FixedDict::initialize(const sl::OBStream& streamInitData, bool isPersistentsStream)
{
	PyFixedDictType::FIXEDDICT_KEYTYPE_MAP& keyTypes = _dataType->getKeyTypes();
	PyFixedDictType::FIXEDDICT_KEYTYPE_MAP::const_iterator iter = keyTypes.begin();

	for(; iter != keyTypes.end(); ++iter)
	{
		/*if(isPersistentsStream && !iter->second->persistent)
		{
			PyObject* val1 = iter->second->dataType->parseDefaultStr("");
			PyDict_SetItemString(pyDict_, iter->first.c_str(), val1);
			
			// 由于PyDict_SetItem会增加引用因此需要减
			Py_DECREF(val1);
		}
		else*/
		{
			PyObject* val1 = NULL;
			/*if(iter->second->dataType->type() == DATA_TYPE_FIXEDDICT)
				val1 = ((FixedDictType*)iter->second->dataType)->createFromStreamEx(streamInitData, isPersistentsStream);
			else if(iter->second->dataType->type() == DATA_TYPE_FIXEDARRAY)
				val1 = ((FixedArrayType*)iter->second->dataType)->createFromStreamEx(streamInitData, isPersistentsStream);
			else*/
			val1 = (PyObject*)(iter->second->createScriptObject(streamInitData));
			PyDict_SetItemString(pyDict_, iter->first.c_str(), val1);
			
			// 由于PyDict_SetItem会增加引用因此需要减
			Py_DECREF(val1);
		}
	}
}

//-------------------------------------------------------------------------------------
PyObject* FixedDict::__py_reduce_ex__(PyObject* self, PyObject* protocol)
{
	FixedDict* fixedDict = static_cast<FixedDict*>(self);
	PyObject* args = PyTuple_New(2);
	PyObject* unpickleMethod = SLMODULE(PythonEngine)->getUnpickleFunc("FixedDict");
	PyTuple_SET_ITEM(args, 0, unpickleMethod);
	PyObject* args1 = PyTuple_New(2);

	PyTuple_SET_ITEM(args1, 0, PyLong_FromLongLong(fixedDict->getPyDataType()->getUid()));
	PyTuple_SET_ITEM(args1, 1, PyDict_Copy(fixedDict->getDictObject()));

	PyTuple_SET_ITEM(args, 1, args1);

	if(unpickleMethod == NULL){
		Py_DECREF(args);
		SLASSERT(false, "wtf");
		return NULL;
	}

	return args;
}

//-------------------------------------------------------------------------------------
PyObject* FixedDict::__unpickle__(PyObject* self, PyObject* args)
{
	Py_ssize_t size = PyTuple_Size(args);
	if(size != 2)
	{
		ECHO_ERROR("FixedDict::__unpickle__: args is wrong! (size != 2)");
		S_Return;
	}

	PyObject* pyDatatypeUID = PyTuple_GET_ITEM(args, 0);
	uint16 uid = (uint16)PyLong_AsUnsignedLong(pyDatatypeUID);

	PyObject* dict = PyTuple_GET_ITEM(args, 1);
	if(dict == NULL)
	{
		ECHO_ERROR("FixedDict::__unpickle__: args is wrong!");
		S_Return;
	}
	
	FixedDict* pFixedDict = new FixedDict((PyDataType*)SLMODULE(ObjectDef)->getDataType(uid));
	pFixedDict->initialize(dict);
	return pFixedDict;
}

//-------------------------------------------------------------------------------------
void FixedDict::onInstallScript(PyObject* mod)
{
	static PyMethodDef __unpickle__Method = 
		{"FixedDict", (PyCFunction)&FixedDict::__unpickle__, METH_VARARGS, 0};

	PyObject* pyFunc = PyCFunction_New(&__unpickle__Method, NULL);
	SLMODULE(PythonEngine)->registerUnpickleFunc(pyFunc, "FixedDict");
	Py_DECREF(pyFunc);
}

//-------------------------------------------------------------------------------------
int FixedDict::mp_length(PyObject* self)
{
	return PyDict_Size(static_cast<FixedDict*>(self)->pyDict_);
}

//-------------------------------------------------------------------------------------
/*
int FixedDict::mp_ass_subscript(PyObject* self, PyObject* key, PyObject* value)
{
	wchar_t* PyUnicode_AsWideCharStringRet0 = PyUnicode_AsWideCharString(key, NULL);
	if (PyUnicode_AsWideCharStringRet0 == NULL)
	{
		char err[255];
		SafeSprintf(err, 255, "FixedDict::mp_ass_subscript: key not is string!\n");
		PyErr_SetString(PyExc_TypeError, err);
		PyErr_PrintEx(0);
		return 0;
	}

	char* dictKeyName = sl::CStringUtils::wchar2char(PyUnicode_AsWideCharStringRet0);
	PyMem_Free(PyUnicode_AsWideCharStringRet0);

	FixedDict* fixedDict = static_cast<FixedDict*>(self);
	if (value == NULL)
	{
		if(!fixedDict->checkDataChanged(dictKeyName, value, true))
		{
			free(dictKeyName);
			return 0;
		}

		free(dictKeyName);
		return PyDict_DelItem(fixedDict->pyDict_, key);
	}
	
	if(!fixedDict->checkDataChanged(dictKeyName, value))
	{
		free(dictKeyName);
		return 0;
	}

	PyObject* val1 = 
		static_cast<PyFixedDictType*>(fixedDict->getPyDataType())->createNewItemFromObj(dictKeyName, value);

	int ret = PyDict_SetItem(fixedDict->pyDict_, key, val1);
	
	// 由于PyDict_SetItem会增加引用因此需要减
	Py_DECREF(val1);

	free(dictKeyName);
	return ret;
}
*/
//-------------------------------------------------------------------------------------
/*
bool FixedDict::checkDataChanged(const char* keyName, PyObject* value, bool isDelete)
{
	FixedDictType::FIXEDDICT_KEYTYPE_MAP& keyTypes = _dataType->getKeyTypes();
	FixedDictType::FIXEDDICT_KEYTYPE_MAP::const_iterator iter = keyTypes.begin();
	
	for(; iter != keyTypes.end(); ++iter)
	{
		if((*iter).first == keyName)
		{
			if(isDelete)
			{
				char err[255];
				kbe_snprintf(err, 255, "can't delete from FIXED_DICT key[%s].\n", keyName);
				PyErr_SetString(PyExc_TypeError, err);
				PyErr_PrintEx(0);
				return false;
			}
			else
			{
				PyDataType* dataType = (*iter).second->dataType;
				if(!dataType->isSameType(value)){
					return false;
				}
			}

			return true;
		}
	}

	char err[255];
	kbe_snprintf(err, 255, "set FIXED_DICT to a unknown key[%s].\n", keyName);
	PyErr_SetString(PyExc_TypeError, err);
	PyErr_PrintEx(0);
	return false;
}
*/	
//-------------------------------------------------------------------------------------
PyObject* FixedDict::mp_subscript(PyObject* self, PyObject* key)
{
	FixedDict* fixedDict = static_cast<FixedDict*>(self);

	PyObject* pyObj = PyDict_GetItem(fixedDict->pyDict_, key);
	if (!pyObj)
		PyErr_SetObject(PyExc_KeyError, key);
	else
		Py_INCREF(pyObj);

	return pyObj;
}

//-------------------------------------------------------------------------------------

PyObject* FixedDict::update(PyObject* args)
{
	PyFixedDictType::FIXEDDICT_KEYTYPE_MAP& keyTypes = _dataType->getKeyTypes();
	PyFixedDictType::FIXEDDICT_KEYTYPE_MAP::const_iterator iter = keyTypes.begin();

	for(; iter != keyTypes.end(); ++iter)
	{
		PyObject* val = PyDict_GetItemString(args, iter->first.c_str());
		if(val)
		{
			PyObject* val1 = 
				static_cast<PyFixedDictType*>(getPyDataType())->createNewItemFromObj(iter->first.c_str(), val);

			PyDict_SetItemString(pyDict_, iter->first.c_str(), val1);
			
			// 由于PyDict_SetItem会增加引用因此需要减
			Py_DECREF(val1);
		}
	}

	S_Return; 
}

//-------------------------------------------------------------------------------------
PyObject* FixedDict::tp_str()
{
	return tp_repr();
}

//-------------------------------------------------------------------------------------
PyObject* FixedDict::tp_repr()
{
	return PyObject_Repr(pyDict_);
}

//-------------------------------------------------------------------------------------
