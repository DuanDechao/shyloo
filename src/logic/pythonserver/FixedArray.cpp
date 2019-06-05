#include "FixedArray.h"
#include "pyscript/py_gc.h"
#include "IPythonEngine.h"
PySequenceMethods FixedArray::seqMethods = {
	seq_length,				// inquiry sq_length;				len(x)
	seq_concat,				// binaryfunc sq_concat;			x + y
	seq_repeat,				// intargfunc sq_repeat;			x * n
	seq_item,				// intargfunc sq_item;				x[i]
	(void*)seq_slice,				// intintargfunc sq_slice;			x[i:j]
    seq_ass_item,			// intobjargproc sq_ass_item;		x[i] = v
	(void*)seq_ass_slice,			// intintobjargproc sq_ass_slice;	x[i:j] = v
	seq_contains,			// objobjproc sq_contains;			v in x
    0, //seq_inplace_concat,		// binaryfunc sq_inplace_concat;	x += y
	0 //seq_inplace_repeat		// intargfunc sq_inplace_repeat;	x *= n
};

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
SCRIPT_INIT(FixedArray, 0, &FixedArray::seqMethods, 0, 0, 0)	
	
FixedArray::FixedArray(PyDataType* dataType, IArray* objArray)
	:ScriptObject(getScriptType(), false),
	 _objArray(objArray)
{
	_dataType = static_cast<PyFixedArrayType*>(dataType);
	SLMODULE(PythonEngine)->incTracing("FixedArray");
}

FixedArray::~FixedArray(){
	SLMODULE(PythonEngine)->decTracing("FixedArray");
}

Py_ssize_t FixedArray::seq_length(PyObject* self){
	FixedArray* fixedArray = static_cast<FixedArray*>(self);
	return fixedArray->length();
}

PyObject* FixedArray::seq_concat(PyObject* self, PyObject* seq){
	if (!PySequence_Check(seq)){
		PyErr_SetString(PyExc_TypeError, "Sequence::seq_concat: argument to + must be a sequence");
		PyErr_PrintEx(0);
		return NULL;
	}
	
	FixedArray* fixedArray = static_cast<FixedArray*>(self);
	int seqSize1 = (int)fixedArray->length();
	int seqSize2 = (int)PySequence_Size(seq);
	PyObject* pyList = PyList_New(seqSize1 + seqSize2);

	for (int i = 0; i < seqSize1; ++i){
		PyObject* pyObj = fixedArray->getDataByIndex(i);
		PyList_SET_ITEM(pyList, i, pyObj);
	}

	for (int i = 0; i < seqSize2; ++i){
		PyList_SET_ITEM(pyList, seqSize1 + i, PySequence_GetItem(seq, i));
	}

	return pyList;
}

PyObject* FixedArray::seq_repeat(PyObject* self, Py_ssize_t n){
	if (n <= 0) 
		return PyList_New(0);

	FixedArray* fixedArray = static_cast<FixedArray*>(self);
	int seqSize1 = (int)fixedArray->length();

	PyObject* pyList = PyList_New(seqSize1 * n);
	// 可能没内存了
	if (pyList == NULL) 
		return NULL;

	for (int j = 0; j < seqSize1; ++j){
		PyObject* pyObj = fixedArray->getDataByIndex(j);
		PyList_SET_ITEM(pyList, j, pyObj);
	}

	for (int i = 1; i < n; i++){
		for (int j = 0; j < seqSize1; ++j){
			PyObject* pyTemp = PyList_GET_ITEM(pyList, j);
			PyList_SET_ITEM(pyList, i * seqSize1 + j, pyTemp);
			Py_INCREF(pyTemp);
		}
	}

	return pyList;
}

PyObject* FixedArray::seq_item(PyObject* self, Py_ssize_t index){
	FixedArray* fixedArray = static_cast<FixedArray*>(self);
	int size = (int)fixedArray->length();
	if (uint32(index) < size){
		PyObject* pyObj = fixedArray->getDataByIndex(index);
		return pyObj;
	}

	PyErr_SetString(PyExc_IndexError, "Sequence index out of range");
	return NULL;
}

PyObject* FixedArray::seq_slice(PyObject* self, Py_ssize_t startIndex, Py_ssize_t endIndex){
	if (startIndex < 0)
		startIndex = 0;

	FixedArray* fixedArray = static_cast<FixedArray*>(self);
	int size = (int)fixedArray->length();
	if (endIndex > size)
		endIndex = size;

	if (endIndex < startIndex)
		endIndex = startIndex;

	int length = (int)(endIndex - startIndex);

	if (length == size) {
		Py_INCREF(self);
		return self;
	}

	PyObject* pyRet = PyList_New(length);
	for (int i = (int)startIndex; i < (int)endIndex; ++i){
		PyObject* pyObj = fixedArray->getDataByIndex(i);
		//Py_INCREF(values[i]);
		PyList_SET_ITEM(pyRet, i - startIndex, pyObj);
	}

	return pyRet;
}

int FixedArray::seq_ass_item(PyObject* self, Py_ssize_t index, PyObject* value){
	FixedArray* fixedArray = static_cast<FixedArray*>(self);
	int size = (int)fixedArray->length();
	if (uint32(index) >= size){
		PyErr_SetString(PyExc_IndexError, "Sequence assignment index out of range");
		PyErr_PrintEx(0);
		return -1;
	}

	if(value){
		// 检查类别是否正确
		if(fixedArray->isSameItemType(value)){
			fixedArray->setDataByIndex(index, value);
		}
		else{
			PyErr_SetString(PyExc_IndexError, "Sequence set to type is error!");
			PyErr_PrintEx(0);
			return -1;
		}
	}
	else{
		fixedArray->removeDataByIndex(index, index);
		//Py_DECREF((*(values.begin() + index)));
		//values.erase(values.begin() + index);
	}

	return 0;
}
int FixedArray::seq_ass_slice(PyObject* self, Py_ssize_t index1, Py_ssize_t index2, PyObject* oterSeq){
	FixedArray* fixedArray = static_cast<FixedArray*>(self);
	// 是否是删除元素
	if (!oterSeq){
		if (index1 < index2){
			fixedArray->removeDataByIndex(index1, index2);
		}
		return 0;
	}

	// oterSeq必须是一个 sequence
	if (!PySequence_Check(oterSeq)){
		PyErr_Format(PyExc_TypeError, "Sequence slices can only be assigned to a sequence");
		PyErr_PrintEx(0);
		return -1;
	}

	if (oterSeq == fixedArray){
		PyErr_Format(PyExc_TypeError, "Sequence does not support assignment of itself to a slice of itself");
		PyErr_PrintEx(0);
		return -1;
	}

	int sz = (int)fixedArray->length();
	int osz = (int)PySequence_Size(oterSeq);

	// 保证index不会越界
	if (index1 > sz) index1 = sz;
	if (index1 < 0) index1 = 0;
	if (index2 > sz) index2 = sz;
	if (index2 < 0) index2 = 0;

	// 检查一下 看看有无错误类别
	for (int i = 0; i < osz; ++i){
		PyObject* pyVal = PySequence_GetItem(oterSeq, i);
		bool ok = fixedArray->isSameItemType(pyVal);
		Py_DECREF(pyVal);

		if (!ok){
			PyErr_Format(PyExc_TypeError, "Array elements must be set to type %s (setting slice %d-%d)", "ss", index1, index2);
			PyErr_PrintEx(0);
			return -1;
		}
	}

	int32 idxCount = index2 - index1;
	if(idxCount > osz){
		fixedArray->removeDataByIndex(index1 + osz, index2);
	}
	else if(idxCount < osz){
		fixedArray->extendByIndex(index2, osz - idxCount);
	}

	for(int i = 0; i < osz; ++i){
		PyObject* pyTemp = PySequence_GetItem(oterSeq, i);
		if(pyTemp == NULL){
			PyErr_Format(PyExc_TypeError, "Sequence::seq_ass_slice::PySequence_GetItem %d is NULL.", i);
			PyErr_PrintEx(0);
		}
		fixedArray->setDataByIndex(index1 + i, pyTemp);

		if (pyTemp)
			Py_DECREF(pyTemp);
	}

	return 0;
}

int FixedArray::seq_contains(PyObject* self, PyObject* value){
	FixedArray* fixedArray = static_cast<FixedArray*>(self);
	return fixedArray->findFrom(0, value) >= 0;
}


PyObject* FixedArray::seq_inplace_concat(PyObject* self, PyObject* oterSeq){
	if (!PySequence_Check(oterSeq)){
		PyErr_SetString(PyExc_TypeError, "Sequence:Argument to += must be a sequence");
		PyErr_PrintEx(0);
		return NULL;
	}

	FixedArray* fixedArray = static_cast<FixedArray*>(self);
	int szA = (int)fixedArray->length();
	int szB = (int)PySequence_Size(oterSeq);

	if (szB == 0) {
		Py_INCREF(fixedArray);
		return fixedArray;
	}

	// 检查类型是否正确
	for (int i = 0; i < szB; ++i){
		PyObject * pyVal = PySequence_GetItem(oterSeq, i);
		bool ok = fixedArray->isSameItemType(pyVal);
		Py_DECREF(pyVal);

		if (!ok){
			PyErr_Format(PyExc_TypeError, "Array elements must be set to type %s (appending with +=)", "sss");
			PyErr_PrintEx(0);
			return NULL;
		}
	}
	
	// 分配好内存
	fixedArray->extendByIndex(szA, szB);	
	for (int i = 0; i < szB; ++i){
		PyObject* pyTemp = PySequence_GetItem(oterSeq, i);
		if(pyTemp == NULL){
			PyErr_Format(PyExc_TypeError, "Sequence::seq_inplace_concat::PySequence_GetItem %d is NULL.", i);
			PyErr_PrintEx(0);
		}

		fixedArray->setDataByIndex(szA + i, pyTemp);
	}

	return fixedArray;
}

PyObject* FixedArray::seq_inplace_repeat(PyObject* self, Py_ssize_t n){
	FixedArray* fixedArray = static_cast<FixedArray*>(self);
	if(n == 1) {
		Py_INCREF(fixedArray);
		return fixedArray;
	}

	fixedArray->repeat(n);
	return fixedArray;
}

void FixedArray::initialize(std::string strInitData){
}

void FixedArray::initialize(PyObject* pyObjInitData){
	if(pyObjInitData == Py_None)
		return;
	
	if(!PySequence_Check(pyObjInitData)){
		return;
	}

	/*
	 Py_ssize_t size = PySequence_Size(pyObjInitData);
	 for(Py_ssize_t i = 0; i < size; ++i){
		 PyObject* pyobj = PySequence_GetItem(pyObjInitData, i);
		 values_.push_back(_dataType->createNewItemFromObj(pyobj));
		 Py_DECREF(pyobj);
	 }
	 */
}

PyObject* FixedArray::getDataByIndex(const int32 index){
	return (PyObject*)(getSubDataType()->createScriptObject(objArray(), index));
}

bool FixedArray::removeDataByIndex(const int32 start, const int32 end){
	return objArray()->remove(start, end);
}

void FixedArray::setDataByIndex(const int32 index, PyObject* pyobj){
	IDataType* subDataType = getSubDataType();
	subDataType->addScriptObject(objArray(), index, pyobj);
}

bool FixedArray::extendByIndex(const int32 index, const int32 count){
	return objArray()->extend(index, count);
}

bool FixedArray::clear(){
	return objArray()->clear();
}

bool FixedArray::repeat(const int32 n){
	return objArray()->repeat(n);
}

PyObject* FixedArray::__py_reduce_ex__(PyObject* self, PyObject* protocol){
	FixedArray* fixedArray = static_cast<FixedArray*>(self);
	PyObject* args = PyTuple_New(2);
	PyObject* unpickleMethod = sl::pyscript::Pickler::getUnpickleFunc("FixedArray");
	PyTuple_SET_ITEM(args, 0, unpickleMethod);
	int len = fixedArray->length();
	PyObject* args1 = PyTuple_New(2);
	PyObject* pyList = PyList_New(len);

	if(len > 0){
		for(int i = 0; i < len; ++i){
			PyObject* pyobj = fixedArray->getDataByIndex(i);
			PyList_SET_ITEM(pyList, i, pyobj);
		}
	}

	PyTuple_SET_ITEM(args1, 0, PyLong_FromLongLong(fixedArray->getDataType()->getUid()));
	PyTuple_SET_ITEM(args1, 1, pyList);
	PyTuple_SET_ITEM(args, 1, args1);

	if(unpickleMethod == NULL){
		Py_DECREF(args);
		return NULL;
	}
	return args;
}

PyObject* FixedArray::__unpickle__(PyObject* self, PyObject* args){
	Py_ssize_t size = PyTuple_Size(args);
	if(size != 2){
		PyErr_Format(PyExc_TypeError, "FixedArray::__unpickle__: args is wrong(size!=2)!");
		PyErr_PrintEx(0);
		S_Return;
	}
	
	PyObject* pyDatatypeUID = PyTuple_GET_ITEM(args, 0);
	uint16 uid = (uint16)PyLong_AsUnsignedLong(pyDatatypeUID);
	PyObject* pyList = PyTuple_GET_ITEM(args, 1);
	if(pyList == NULL){
		PyErr_Format(PyExc_TypeError, "FixedArray::__unpickle__: args is wrong!");
		PyErr_PrintEx(0);
		S_Return;
	}
	
	FixedArray* pFixedArray = NEW FixedArray((PyDataType*)SLMODULE(ObjectDef)->getDataType(uid));
	pFixedArray->initialize(pyList);
	return pFixedArray;
}

void FixedArray::onInstallScript(PyObject* mod){
	static PyMethodDef __unpickle__Method = {"FixedArray", (PyCFunction)&FixedArray::__unpickle__, METH_VARARGS, 0};
	PyObject* pyFunc = PyCFunction_New(&__unpickle__Method, NULL);
	SLMODULE(PythonEngine)->registerUnpickleFunc(pyFunc, "FixedArray");
	Py_DECREF(pyFunc);
}

bool FixedArray::isSameType(PyObject* pyValue){
	return true;
	//return _dataType->isSameType(pyValue);
}

bool FixedArray::isSameItemType(PyObject* pyValue){
	return true;
	//return _dataType->isSameItemType(pyValue);
}

int FixedArray::findFrom(uint32 startIndex, PyObject* value){
	for (uint32 i = startIndex; i < length(); ++i){
		PyObject* pyobj = getDataByIndex(i);
		if(PyObject_RichCompareBool(value, pyobj, Py_EQ)) 
			return i;
	}	

	return -1;
}

PyObject* FixedArray::createNewItemFromObj(PyObject* pyItem){
	return _dataType->createNewItemFromObj(pyItem);
}

PyObject* FixedArray::__py_append(PyObject* self, PyObject* args, PyObject* kwargs){
	FixedArray* ary = static_cast<FixedArray*>(self);
	uint32 seq_size = ary->length();
	return PyBool_FromLong(seq_ass_slice(self, seq_size, seq_size, &*args) == 0);	
}

PyObject* FixedArray::__py_count(PyObject* self, PyObject* args, PyObject* kwargs){
	FixedArray* ary = static_cast<FixedArray*>(self);
	PyObject* pyItem = PyTuple_GetItem(args, 0);
	int count = 0, cur;
	for (uint32 i = 0; (cur = ary->findFrom(i, &*pyItem)) >= 0; i = cur + 1)
		++count;
	return PyLong_FromLong(count);	
}

PyObject* FixedArray::__py_extend(PyObject* self, PyObject* args, PyObject* kwargs){
	FixedArray* ary = static_cast<FixedArray*>(self);
	uint32 seq_size = ary->length();
	PyObject* pyItem = PyTuple_GetItem(args, 0);
	return PyBool_FromLong(seq_ass_slice(self, seq_size, seq_size, &*pyItem) == 0);	
}

PyObject* FixedArray::__py_index(PyObject* self, PyObject* args, PyObject* kwargs){
	FixedArray* ary = static_cast<FixedArray*>(self);
	PyObject* pyItem = PyTuple_GetItem(args, 0);
	int index = ary->findFrom(0, &*pyItem);
	if (index == -1){
		PyErr_SetString(PyExc_ValueError, "FixedArray::index: value not found");
		return NULL;
	}

	return PyLong_FromLong(index);
}

PyObject* FixedArray::__py_insert(PyObject* self, PyObject* args, PyObject* kwargs){
	const int argsize = (int)PyTuple_Size(args);
	if (argsize != 2){
		PyErr_SetString(PyExc_ValueError, "FixedArray::insert(): takes exactly 2 arguments (array.insert(i, x))");
		return NULL;
	}

	int before = PyLong_AsLong(PyTuple_GetItem(args, 0));
	PyObject* pyobj = PyTuple_GetItem(args, 1);
	
	PyObject* pyTuple = PyTuple_New(1);

	Py_INCREF(pyobj);
	PyTuple_SET_ITEM(&*pyTuple, 0, pyobj);
	
	PyObject* ret = PyBool_FromLong(seq_ass_slice(self, before, before, &*pyTuple) == 0);
	Py_DECREF(pyTuple);

	return ret;
}

PyObject* FixedArray::__py_pop(PyObject* self, PyObject* args, PyObject* kwargs){
	FixedArray* ary = static_cast<FixedArray*>(self);
	int32 length = ary->length();
	if (length <= 0){
		PyErr_SetString(PyExc_IndexError, "FixedArray.pop: empty array");
		return NULL;
	}

	PyObject* pyItem = PyTuple_GetItem(args, 0);
	int index = PyLong_AsLong(pyItem);
	if (index < 0) {
		index += (int)length;
	}

	if (uint32(index) >= length){
		PyErr_SetString(PyExc_IndexError, "FixedArray.pop: index out of range");
		return NULL;
	}
	PyObject* pyValue = ary->getDataByIndex(index);

	PyObject* pyTuple = PyTuple_New(0);
	if (seq_ass_slice(self, index, index + 1, &*pyTuple) != 0){
		Py_DECREF(pyTuple);
		return NULL;
	}

	Py_DECREF(pyTuple);
	return pyValue;
}

PyObject* FixedArray::__py_remove(PyObject* self, PyObject* args, PyObject* kwargs){
	FixedArray* ary = static_cast<FixedArray*>(self);
	PyObject* pyItem = PyTuple_GetItem(args, 0);

	int index = ary->findFrom(0, &*pyItem);
	if (index == -1){
		PyErr_SetString(PyExc_ValueError, "FixedArray.remove: value not found");
		return PyLong_FromLong(-1);
	}

	PyObject* pyTuple = PyTuple_New(0);
	PyObject* ret = PyBool_FromLong(seq_ass_slice(self, index, index + 1, &*pyTuple) == 0);
	Py_DECREF(pyTuple);
	return ret;
}

PyObject* FixedArray::tp_str(){
	return tp_repr();
}

PyObject* FixedArray::tp_repr(){
	int32 size = length();
	PyObject* pyList = PyList_New(size);
	
	for(size_t i = 0; i < size; ++i){
		PyObject* pyobj = getDataByIndex(i);
		PyList_SET_ITEM(pyList, i, pyobj);
	}

	PyObject* pyStr = PyObject_Repr(pyList);
	Py_DECREF(pyList);

	return pyStr;
}
