#include "EntityGarbages.h"
/** 
	Python EntityGarbages操作所需要的方法表 
*/
PyMappingMethods EntityGarbages::mappingMethods =
{
	(lenfunc)mp_length,								// mp_length
	(binaryfunc)mp_subscript,						// mp_subscript
	NULL											// mp_ass_subscript
};

// 参考 objects/dictobject.c
// Hack to implement "key in dict"
PySequenceMethods EntityGarbages::mappingSequenceMethods = 
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

SCRIPT_METHOD_DECLARE_BEGIN(EntityGarbages)
SCRIPT_METHOD_DECLARE("has_key",			pyHas_key,		METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("keys",				pyKeys,			METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("values",				pyValues,		METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("items",				pyItems,		METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("get",				pyGet,			METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_MEMBER_DECLARE_BEGIN(EntityGarbages)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(EntityGarbages)
SCRIPT_GETSET_DECLARE_END()
SCRIPT_INIT(EntityGarbages, 0, &EntityGarbages::mappingSequenceMethods, &EntityGarbages::mappingMethods, 0, 0)	

//-------------------------------------------------------------------------------------
int EntityGarbages::mp_length(PyObject * self){
	return (int)static_cast<EntityGarbages*>(self)->getEntities().size();
}
	
//-------------------------------------------------------------------------------------
PyObject* EntityGarbages::mp_subscript(PyObject* self, PyObject* key /*entityID*/){
	EntityGarbages* lpEntities = static_cast<EntityGarbages*>(self);
	uint64 entityID = PyLong_AsLong(key);
	if (PyErr_Occurred())
		return NULL;

	PyObject * pyEntity = NULL;

	ENTITYS_MAP& entities = lpEntities->getEntities();
	ENTITYS_MAP::const_iterator iter = entities.find(entityID);
	if (iter != entities.end())
		pyEntity = iter->second;

	if(pyEntity == NULL)
	{
		PyErr_Format(PyExc_KeyError, "%d", entityID);
		//PyErr_PrintEx(0);
		return NULL;
	}

	Py_INCREF(pyEntity);
	return pyEntity;
}

//-------------------------------------------------------------------------------------
PyObject* EntityGarbages::pyHas_key(uint64 entityID){
	ENTITYS_MAP& entities = getEntities();
	return PyLong_FromLong((entities.find(entityID) != entities.end()));
}

//-------------------------------------------------------------------------------------
PyObject* EntityGarbages::pyKeys(){
	ENTITYS_MAP& entities = getEntities();
	PyObject* pyList = PyList_New(entities.size());
	int i = 0;

	ENTITYS_MAP::const_iterator iter = entities.begin();
	while (iter != entities.end()){
		PyObject* entityID = PyLong_FromLong(iter->first);
		PyList_SET_ITEM(pyList, i, entityID);

		i++;
		iter++;
	}

	return pyList;
}

//-------------------------------------------------------------------------------------
PyObject* EntityGarbages::pyValues(){
	ENTITYS_MAP& entities = getEntities();
	PyObject* pyList = PyList_New(entities.size());
	int i = 0;

	ENTITYS_MAP::const_iterator iter = entities.begin();
	while (iter != entities.end()){
		Py_INCREF(iter->second);
		PyList_SET_ITEM(pyList, i, iter->second);

		i++;
		iter++;
	}

	return pyList;
}

//-------------------------------------------------------------------------------------
PyObject* EntityGarbages::pyItems(){
	ENTITYS_MAP& entities = getEntities();
	PyObject* pyList = PyList_New(entities.size());
	int i = 0;

	ENTITYS_MAP::const_iterator iter = entities.begin();
	while (iter != entities.end()){
		PyObject * pTuple = PyTuple_New(2);
		PyObject* entityID = PyLong_FromLong(iter->first);
		Py_INCREF(iter->second);

		PyTuple_SET_ITEM(pTuple, 0, entityID);
		PyTuple_SET_ITEM(pTuple, 1, iter->second);
		PyList_SET_ITEM(pyList, i, pTuple);
		i++;
		iter++;
	}

	return pyList;
}

//-------------------------------------------------------------------------------------
PyObject* EntityGarbages::__py_pyGet(PyObject* self, PyObject * args, PyObject* kwds){
	EntityGarbages* lpEntities = static_cast<EntityGarbages*>(self);
	PyObject * pDefault = Py_None;
	uint64 id = 0;
	if (!PyArg_ParseTuple( args, "i|O", &id, &pDefault)){
		return NULL;
	}

	PyObject* pEntity = lpEntities->find(id);

	if (!pEntity){
		pEntity = pDefault;
	}

	Py_INCREF(pEntity);
	return pEntity;
}

//-------------------------------------------------------------------------------------
void EntityGarbages::add(uint64 id, EntityScriptObject* entity){ 
	ENTITYS_MAP::const_iterator iter = _entities.find(id);
	if(iter != _entities.end()){
		ECHO_ERROR("EntityGarbages::add: entityID:%lld has exist.", id);
		return;
	}

	if(_entities.size() == 0){
		_lastTime = sl::getTimeMilliSecond();
	}
	else
	{
		// X秒内没有清空过garbages则错误警告
		if(_lastTime > 0 && sl::getTimeMilliSecond() - _lastTime > 3600 * 1000){
			// 再未清空情况下，下次不提示了
			_lastTime = 0;
			
			ECHO_ERROR("For a long time(3600s) not to empty the garbages, there may be a leak of the entitys(size:%d), "
				"please use the \"KBEngine.entities.garbages.items()\" command query!", 
				size());
		}
	}
	
	_entities[id] = entity; 
}

//-------------------------------------------------------------------------------------
void EntityGarbages::clear(){
	_entities.clear();
	_lastTime = 0;
}

//-------------------------------------------------------------------------------------
EntityScriptObject* EntityGarbages::find(uint64 id){
	ENTITYS_MAP::const_iterator iter = _entities.find(id);
	if(iter != _entities.end()){
		return iter->second;
	}
	
	return NULL;
}

//-------------------------------------------------------------------------------------
void EntityGarbages::erase(uint64 id){
	_entities.erase(id);
	
	if(_entities.size() == 0)
		_lastTime = 0;
}

