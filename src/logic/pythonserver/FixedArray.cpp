#include "FixedArray.h"
#include "pyscript/py_gc.h"
#include "IPythonEngine.h"
SCRIPT_METHOD_DECLARE_BEGIN(FixedArray)
SCRIPT_METHOD_DECLARE("__reduce_ex__",				reduce_ex__,			METH_VARARGS, 0)
SCRIPT_METHOD_DECLARE("append",						append,					METH_VARARGS, 0)
SCRIPT_METHOD_DECLARE("count",						count,					METH_VARARGS, 0)
SCRIPT_METHOD_DECLARE("extend",						extend,					METH_VARARGS, 0)
SCRIPT_METHOD_DECLARE("index",						index,					METH_VARARGS, 0)
SCRIPT_METHOD_DECLARE("insert",						insert,					METH_VARARGS, 0)
SCRIPT_METHOD_DECLARE("pop",						pop,					METH_VARARGS, 0)
SCRIPT_METHOD_DECLARE("remove",						remove,					METH_VARARGS, 0)
SCRIPT_METHOD_DECLARE_END()


SCRIPT_MEMBER_DECLARE_BEGIN(FixedArray)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(FixedArray)
SCRIPT_GETSET_DECLARE_END()
SCRIPT_INIT(FixedArray, 0, &Sequence::seqMethods, 0, 0, 0)	
	
//-------------------------------------------------------------------------------------
FixedArray::FixedArray(PyDataType* dataType):
Sequence(getScriptType(), false)
{
	_dataType = static_cast<PyFixedArrayType*>(dataType);
//	_dataType->incRef();

	SLMODULE(PythonEngine)->incTracing("FixedArray");

//	DEBUG_MSG(fmt::format("FixedArray::FixedArray(): {:p}\n", (void*)this));
}

//-------------------------------------------------------------------------------------
FixedArray::~FixedArray()
{
	//_dataType->decRef();

	SLMODULE(PythonEngine)->decTracing("FixedArray");

//	DEBUG_MSG(fmt::format("FixedArray::~FixedArray(): {:p}\n", (void*)this));
}

//-------------------------------------------------------------------------------------
void FixedArray::initialize(std::string strInitData)
{
}

//-------------------------------------------------------------------------------------
void FixedArray::initialize(PyObject* pyObjInitData)
{
	if(pyObjInitData == Py_None)
		return;
	
	if(!PySequence_Check(pyObjInitData))
	{
		return;
	}

	 Py_ssize_t size = PySequence_Size(pyObjInitData);
	 for(Py_ssize_t i=0; i<size; ++i)
	 {
		 PyObject* pyobj = PySequence_GetItem(pyObjInitData, i);
		 values_.push_back(_dataType->createNewItemFromObj(pyobj));
		 Py_DECREF(pyobj);
	 }
}

//-------------------------------------------------------------------------------------
PyObject* FixedArray::__py_reduce_ex__(PyObject* self, PyObject* protocol)
{
	FixedArray* arr = static_cast<FixedArray*>(self);
	PyObject* args = PyTuple_New(2);
	PyObject* unpickleMethod = sl::pyscript::Pickler::getUnpickleFunc("FixedArray");
	PyTuple_SET_ITEM(args, 0, unpickleMethod);
	int len = arr->length();
	PyObject* args1 = PyTuple_New(2);
	PyObject* pyList = PyList_New(len);

	if(len > 0)
	{
		std::vector<PyObject*>& values = arr->getValues();
		for(int i=0; i<len; ++i)
		{
			Py_INCREF(values[i]);
			PyList_SET_ITEM(pyList, i, values[i]);
		}
	}

	PyTuple_SET_ITEM(args1, 0, PyLong_FromLongLong(arr->getDataType()->getUid()));
	PyTuple_SET_ITEM(args1, 1, pyList);

	PyTuple_SET_ITEM(args, 1, args1);

	if(unpickleMethod == NULL){
		Py_DECREF(args);
		return NULL;
	}
	return args;
}

//-------------------------------------------------------------------------------------
PyObject* FixedArray::__unpickle__(PyObject* self, PyObject* args)
{
	Py_ssize_t size = PyTuple_Size(args);
	if(size != 2)
	{
		ECHO_ERROR("FixedArray::__unpickle__: args is wrong! (size != 2)");
		S_Return;
	}
	
	PyObject* pyDatatypeUID = PyTuple_GET_ITEM(args, 0);
	uint16 uid = (uint16)PyLong_AsUnsignedLong(pyDatatypeUID);
	PyObject* pyList = PyTuple_GET_ITEM(args, 1);
	if(pyList == NULL)
	{
		ECHO_ERROR("FixedArray::__unpickle__: args is wrong!");
		S_Return;
	}
	
	FixedArray* pFixedArray = new FixedArray((PyDataType*)SLMODULE(ObjectDef)->getDataType(uid));
	pFixedArray->initialize(pyList);
	return pFixedArray;
}

//-------------------------------------------------------------------------------------
void FixedArray::onInstallScript(PyObject* mod)
{
	static PyMethodDef __unpickle__Method = {"FixedArray", (PyCFunction)&FixedArray::__unpickle__, METH_VARARGS, 0};
	PyObject* pyFunc = PyCFunction_New(&__unpickle__Method, NULL);
	SLMODULE(PythonEngine)->registerUnpickleFunc(pyFunc, "FixedArray");
	Py_DECREF(pyFunc);
}

//-------------------------------------------------------------------------------------
bool FixedArray::isSameType(PyObject* pyValue)
{
//	return _dataType->isSameType(pyValue);
	return true;
}

//-------------------------------------------------------------------------------------
bool FixedArray::isSameItemType(PyObject* pyValue)
{
	//return _dataType->isSameItemType(pyValue);
	return true;
}

//-------------------------------------------------------------------------------------
PyObject* FixedArray::createNewItemFromObj(PyObject* pyItem)
{
	return _dataType->createNewItemFromObj(pyItem);
}

//-------------------------------------------------------------------------------------
PyObject* FixedArray::__py_append(PyObject* self, PyObject* args, PyObject* kwargs)
{
	FixedArray* ary = static_cast<FixedArray*>(self);
	uint32 seq_size = ary->length();
	return PyBool_FromLong(seq_ass_slice(self, seq_size, seq_size, &*args) == 0);	
}

//-------------------------------------------------------------------------------------
PyObject* FixedArray::__py_count(PyObject* self, PyObject* args, PyObject* kwargs)
{
	FixedArray* ary = static_cast<FixedArray*>(self);
	PyObject* pyItem = PyTuple_GetItem(args, 0);
	int count = 0, cur;
	for (uint32 i = 0; (cur = ary->findFrom(i, &*pyItem)) >= 0; i = cur + 1)
		++count;
	return PyLong_FromLong(count);	
}

//-------------------------------------------------------------------------------------
PyObject* FixedArray::__py_extend(PyObject* self, PyObject* args, PyObject* kwargs)
{
	FixedArray* ary = static_cast<FixedArray*>(self);
	uint32 seq_size = ary->length();
	PyObject* pyItem = PyTuple_GetItem(args, 0);
	return PyBool_FromLong(seq_ass_slice(self, seq_size, seq_size, &*pyItem) == 0);	
}

//-------------------------------------------------------------------------------------
PyObject* FixedArray::__py_index(PyObject* self, PyObject* args, PyObject* kwargs)
{
	FixedArray* ary = static_cast<FixedArray*>(self);
	PyObject* pyItem = PyTuple_GetItem(args, 0);
	int index = ary->findFrom(0, &*pyItem);
	if (index == -1)
	{
		PyErr_SetString(PyExc_ValueError, "FixedArray::index: value not found");
		return NULL;
	}

	return PyLong_FromLong(index);
}

//-------------------------------------------------------------------------------------
PyObject* FixedArray::__py_insert(PyObject* self, PyObject* args, PyObject* kwargs)
{
	const int argsize = (int)PyTuple_Size(args);
	if (argsize != 2)
	{
		PyErr_SetString(PyExc_ValueError, "FixedArray::insert(): takes exactly 2 arguments (array.insert(i, x))");
		return NULL;
	}

	int before = PyLong_AsLong(PyTuple_GetItem(args, 0));
	PyObject* pyobj = PyTuple_GetItem(args, 1);
	
	//FixedArray* ary = static_cast<FixedArray*>(self);
	PyObject* pyTuple = PyTuple_New(1);

	Py_INCREF(pyobj);
	PyTuple_SET_ITEM(&*pyTuple, 0, pyobj);
	
	PyObject* ret = PyBool_FromLong(seq_ass_slice(self, before, before, &*pyTuple) == 0);
	Py_DECREF(pyTuple);

	return ret;
}

//-------------------------------------------------------------------------------------
PyObject* FixedArray::__py_pop(PyObject* self, PyObject* args, PyObject* kwargs)
{
	FixedArray* ary = static_cast<FixedArray*>(self);
	std::vector<PyObject*>& values = ary->getValues();
	
	if (values.empty())
	{
		PyErr_SetString(PyExc_IndexError, "FixedArray.pop: empty array");
		return NULL;
	}

	PyObject* pyItem = PyTuple_GetItem(args, 0);
	int index = PyLong_AsLong(pyItem);
	if (index < 0) index += (int)values.size();
	if (uint32(index) >= values.size())
	{
		PyErr_SetString(PyExc_IndexError, "FixedArray.pop: index out of range");
		return NULL;
	}

	PyObject* pyValue = values[index];
	Py_INCREF(pyValue);

	PyObject* pyTuple = PyTuple_New(0);

	if (seq_ass_slice(self, index, index + 1, &*pyTuple) != 0)
	{
		Py_DECREF(pyTuple);
		return NULL;
	}

	Py_DECREF(pyTuple);
	return pyValue;
}

//-------------------------------------------------------------------------------------
PyObject* FixedArray::__py_remove(PyObject* self, PyObject* args, PyObject* kwargs)
{
	FixedArray* ary = static_cast<FixedArray*>(self);
	PyObject* pyItem = PyTuple_GetItem(args, 0);
	int index = ary->findFrom(0, &*pyItem);
	if (index == -1)
	{
		PyErr_SetString(PyExc_ValueError, "FixedArray.remove: value not found");
		return PyLong_FromLong(-1);
	}

	PyObject* pyTuple = PyTuple_New(0);
	PyObject* ret = PyBool_FromLong(seq_ass_slice(self, index, index + 1, &*pyTuple) == 0);
	Py_DECREF(pyTuple);
	return ret;
}

//-------------------------------------------------------------------------------------
PyObject* FixedArray::tp_str()
{
	return tp_repr();
}

//-------------------------------------------------------------------------------------
PyObject* FixedArray::tp_repr()
{
	std::vector<PyObject*>& values = getValues();
	PyObject* pyList = PyList_New(values.size());
	
	for(size_t i=0; i<values.size(); ++i)
	{
		Py_INCREF(values[i]);
		PyList_SET_ITEM(pyList, i, values[i]);
	}

	PyObject* pyStr = PyObject_Repr(pyList);
	Py_DECREF(pyList);

	return pyStr;
}

//-------------------------------------------------------------------------------------
