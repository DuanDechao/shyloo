#ifndef SL_LOGIC_ENTITY_MGR_H
#define SL_LOGIC_ENTITY_MGR_H
#include "slikernel.h"
#include "IEntityMgr.h"
#include "IEntityDef.h"
class IBase;
class EntityMgr : public IEntityMgr{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual IObject* createBase(const char* entityType, PyObject* params);

	static PyObject* __py__createBase(PyObject* self, PyObject* args);

	void test();

private:
	template<typename E>
	IObject* createEntity(const char* entityType, PyObject* params);

	template<typename E>
	E* onCreateEntity(PyObject* obj, IScriptDefModule* defModule);

private:
	static EntityMgr*		s_self;
	sl::api::IKernel*		_kernel;
};
#endif