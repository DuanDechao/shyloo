#ifndef SL_ENTITYGARBAGES_H
#define SL_ENTITYGARBAGES_H
	
// common include	
#include "pyscript/scriptobject.h"
#include <unordered_map>
#include "sltime.h"
#include "EntityScriptObject.h"
class EntityGarbages : public sl::pyscript::ScriptObject{
	/** 
		子类化 将一些py操作填充进派生类 
	*/
	INSTANCE_SCRIPT_HREADER(EntityGarbages, ScriptObject)	
public:
	typedef std::unordered_map<uint64, EntityScriptObject*> ENTITYS_MAP;

	EntityGarbages():
	ScriptObject(getScriptType(), false),
	_entities(),
	_lastTime(0)
	{			
	}

	~EntityGarbages()
	{
		if(size() > 0)
		{
			ECHO_ERROR("EntityGarbages::~EntityGarbages(): leaked, size=%s.", 
				size());

			int i = 0;

			ENTITYS_MAP::iterator iter = _entities.begin();
			for(; iter != _entities.end(); ++iter)
			{
				if(i++ >= 256)
					break;

				ECHO_ERROR("--> leaked: %s(%lld).", 
					iter->second->ob_type->tp_name, iter->first);
			}
		}

		finalise();
	}	

	void finalise()
	{
		clear();
	}

	/** 
		暴露一些字典方法给python 
	*/
	DECLARE_PY_MOTHOD_ARG1(pyHas_key, uint64);
	DECLARE_PY_MOTHOD_ARG0(pyKeys);
	DECLARE_PY_MOTHOD_ARG0(pyValues);
	DECLARE_PY_MOTHOD_ARG0(pyItems);
	
	static PyObject* __py_pyGet(PyObject * self, 
		PyObject * args, PyObject* kwds);

	/** 
		map操作函数相关 
	*/
	static PyObject* mp_subscript(PyObject* self, PyObject* key);

	static int mp_length(PyObject* self);

	static PyMappingMethods mappingMethods;
	static PySequenceMethods mappingSequenceMethods;

	ENTITYS_MAP& getEntities(void){ return _entities; }

	void add(uint64 id, EntityScriptObject* entity);
	void clear();
	void erase(uint64 id);

	EntityScriptObject* find(uint64 id);

	size_t size() const { return _entities.size(); }
		
private:
	ENTITYS_MAP _entities;
	uint64 _lastTime;
};
#endif // KBE_ENTITYGARBAGES_H

