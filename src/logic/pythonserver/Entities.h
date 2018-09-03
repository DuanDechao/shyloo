#ifndef SL_ENTITIES_H
#define SL_ENTITIES_H
	
// common include	
#include "pyscript/scriptobject.h"
#include "EntityGarbages.h"
#include <unordered_map>
#include "EntityScriptObject.h"
class Entities : public sl::pyscript::ScriptObject{
	/** 
		���໯ ��һЩpy�������������� 
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
		��¶һЩ�ֵ䷽����python 
	*/
	DECLARE_PY_MOTHOD_ARG1(pyHas_key, uint64);
	DECLARE_PY_MOTHOD_ARG0(pyKeys);
	DECLARE_PY_MOTHOD_ARG0(pyValues);
	DECLARE_PY_MOTHOD_ARG0(pyItems);
	
	static PyObject* __py_pyGet(PyObject * self, 
		PyObject * args, PyObject* kwds);

	/** 
		map����������� 
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

	// �Ѿ����ù�destroy��δ������entity�����洢����� ��ʱ��δ������˵��
	// �ű����п��ܴ���ѭ�����õ���������ڴ�й¶��
	EntityGarbages* _pGarbages;
};
#endif
