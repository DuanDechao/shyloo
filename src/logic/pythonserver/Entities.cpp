#include "Entities.h"

PyMappingMethods Entities::mappingMethods =
{
	(lenfunc)mp_length,								// mp_length
	(binaryfunc)mp_subscript,						// mp_subscript
	NULL											// mp_ass_subscript
};

// 参考 objects/dictobject.c
// Hack to implement "key in dict"
PySequenceMethods Entities::mappingSequenceMethods = 
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

SCRIPT_METHOD_DECLARE_BEGIN(Entities)
SCRIPT_METHOD_DECLARE("has_key",			pyHas_key,				METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("keys",				pyKeys,					METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("values",				pyValues,				METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("items",				pyItems,				METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE("get",				pyGet,					METH_VARARGS,		0)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_MEMBER_DECLARE_BEGIN(Entities)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(Entities)
SCRIPT_GET_DECLARE("garbages",				pyGarbages,				0,							0)
SCRIPT_GETSET_DECLARE_END()
SCRIPT_INIT(Entities, 0, &Entities::mappingSequenceMethods, &Entities::mappingMethods, 0, 0)	


//-------------------------------------------------------------------------------------
int Entities::mp_length(PyObject * self){
	return (int)static_cast<Entities*>(self)->getEntities().size();
}
	
//-------------------------------------------------------------------------------------
PyObject * Entities::mp_subscript(PyObject* self, PyObject* key /*entityID*/){
	Entities* lpEntities = static_cast<Entities*>(self);
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

PyObject* Entities::pyHas_key(uint64 entityID)
{
	ENTITYS_MAP& entities = getEntities();
	return PyLong_FromLong((entities.find(entityID) != entities.end()));
}

//-------------------------------------------------------------------------------------

PyObject* Entities::pyKeys()
{
	ENTITYS_MAP& entities = getEntities();
	PyObject* pyList = PyList_New(entities.size());
	int i = 0;

	ENTITYS_MAP::const_iterator iter = entities.begin();
	while (iter != entities.end())
	{
		PyObject* entityID = PyLong_FromLong(iter->first);
		PyList_SET_ITEM(pyList, i, entityID);

		i++;
		iter++;
	}

	return pyList;
}

//-------------------------------------------------------------------------------------

PyObject* Entities::pyValues()
{
	ENTITYS_MAP& entities = getEntities();
	PyObject* pyList = PyList_New(entities.size());
	int i = 0;

	ENTITYS_MAP::const_iterator iter = entities.begin();
	while (iter != entities.end())
	{
		Py_INCREF(iter->second);
		PyList_SET_ITEM(pyList, i, iter->second);

		i++;
		iter++;
	}

	return pyList;
}

//-------------------------------------------------------------------------------------

PyObject* Entities::pyItems()
{
	ENTITYS_MAP& entities = getEntities();
	PyObject* pyList = PyList_New(entities.size());
	int i = 0;

	ENTITYS_MAP::const_iterator iter = entities.begin();
	while (iter != entities.end())
	{
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

PyObject* Entities::__py_pyGet(PyObject* self, PyObject * args, PyObject* kwds)
{
	Entities* lpEntities = static_cast<Entities*>(self);
	PyObject * pDefault = Py_None;
	uint64 id = 0;
	if (!PyArg_ParseTuple( args, "i|O", &id, &pDefault))
	{
		return NULL;
	}

	PyObject* pEntity = lpEntities->find(id);

	if (!pEntity)
	{
		pEntity = pDefault;
	}

	Py_INCREF(pEntity);
	return pEntity;
}

//-------------------------------------------------------------------------------------

void Entities::add(uint64 id, EntityScriptObject* entity)
{ 
	ENTITYS_MAP::const_iterator iter = _entities.find(id);
	if(iter != _entities.end())
	{
		ECHO_ERROR("Entities::add: entityID:%lld has exist.", id);
		return;
	}

	_entities[id] = entity; 
}

//-------------------------------------------------------------------------------------

void Entities::clear(bool callScript)
{
	ENTITYS_MAP::const_iterator iter = _entities.begin();
	while (iter != _entities.end())
	{
		EntityScriptObject* entity = iter->second;
		_pGarbages->add(entity->id(), entity);
		//entity->destroy(callScript);
		iter++;
	}

	_entities.clear();
}

//-------------------------------------------------------------------------------------

void Entities::clear(bool callScript, std::vector<uint64> excludes)
{
	ENTITYS_MAP::const_iterator iter = _entities.begin();
	for (;iter != _entities.end();)
	{
		if(std::find(excludes.begin(), excludes.end(), iter->first) != excludes.end())
		{
			++iter;
			continue;
		}

		EntityScriptObject* entity = iter->second;
		_pGarbages->add(entity->id(), entity);
		//entity->destroy(callScript);
		_entities.erase(iter++);
	}
	
	// 由于存在excludes不能清空
	// _entities.clear();
}

//-------------------------------------------------------------------------------------

EntityScriptObject* Entities::find(uint64 id)
{
	ENTITYS_MAP::const_iterator iter = _entities.find(id);
	if(iter != _entities.end())
	{
		return iter->second;
	}
	
	return NULL;
}

//-------------------------------------------------------------------------------------

EntityScriptObject* Entities::erase(uint64 id)
{
	ENTITYS_MAP::iterator iter = _entities.find(id);
	if(iter != _entities.end())
	{
		EntityScriptObject* entity = iter->second;
		_pGarbages->add(id, entity);
		_entities.erase(iter);
		return entity;
	}
	
	return NULL;
}

//-------------------------------------------------------------------------------------

PyObject* Entities::pyGarbages(){ 
	if(_pGarbages == NULL)
		S_Return;

	Py_INCREF(_pGarbages);
	return _pGarbages; 
}

