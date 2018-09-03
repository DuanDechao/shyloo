#ifndef SL_ENTITIES_H
#define SL_ENTITIES_H
	
// common include	
#include "pyscript/scriptobject.h"
#include "EntityGarbages.h"
#include <unordered_map>
#include "EntityScriptObject.h"
class Entities : public sl::pyscript::ScriptObject{
	/** 
		子类化 将一些py操作填充进派生类 
	*/
	//BASE_SCRIPT_HREADERINSTANCE_SCRIPT_HREADER(Entities, ScriptObject)	
	BASE_SCRIPT_HREADER(Entities, ScriptObject)	
public:
	typedef unordered_map<uint64, EntityScriptObject*> ENTITYS_MAP;

	Entities():
	ScriptObject(getScriptType(), false),
	_pGarbages(new EntityGarbages())
	{			
	}

	~Entities()
	{
		finalise();
		S_RELEASE(_pGarbages);
	}	

	void finalise()
	{
		clear(false);
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
	static PyObject* mp_subscript(PyObject * self, PyObject * key);

	static int mp_length(PyObject * self);

	static PyMappingMethods mappingMethods;
	static PySequenceMethods mappingSequenceMethods;

	ENTITYS_MAP& getEntities(void) { return _entities; }

	void add(uint64 id, EntityScriptObject* entity);
	void clear(bool callScript);
	void clear(bool callScript, std::vector<uint64> excludes);
	EntityScriptObject* erase(uint64 id);

	EntityScriptObject* find(uint64 id);

	size_t size() const { return _entities.size(); }

	EntityGarbages* pGetbages() { return _pGarbages; }
	DECLARE_PY_GET_MOTHOD(pyGarbages);

private:
	ENTITYS_MAP _entities;

	// 已经调用过destroy但未析构的entity都将存储在这里， 长时间未被析构说明
	// 脚本层有可能存在循环引用的问题造成内存泄露。
	EntityGarbages* _pGarbages;
};
#endif
