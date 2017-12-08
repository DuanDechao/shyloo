#include "EntityMgr.h"
#include "IEntityDef.h"
#include "Base.h"
#include "IPythonEngine.h"
#include "IDCCenter.h"

EntityMgr* EntityMgr::s_self = nullptr;
bool EntityMgr::initialize(sl::api::IKernel * pKernel){
	s_self = this;
	SLMODULE(EntityDef)->rgsBaseScriptModule(Base::getScriptType());
	Base::installScript("Base");

	INSTALL_SCRIPT_MODULE_METHOD(SLMODULE(PythonEngine), "createBase", &EntityMgr::__py__createBase);
	
	return true;
}

bool EntityMgr::launched(sl::api::IKernel * pKernel){
	test();
	return true;
}

bool EntityMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

IObject* EntityMgr::createBase(const char* entityType, PyObject* params){
	return createEntity<Base>(entityType, params);
}

template<typename E>
IObject* EntityMgr::createEntity(const char* entityType, PyObject* params){
	IScriptDefModule* defModule = SLMODULE(EntityDef)->findScriptDefModule(entityType);
	if (defModule == NULL){
		SLASSERT(false, "wtf");
		return NULL;
	}
	else{

	}

	PyObject* obj = defModule->createObject();
	E* entity = onCreateEntity<E>(obj, defModule);
	if (!entity)
		return NULL;

	entity->initializeScript();

	return defModule->getMMObject(static_cast<PyObject*>(entity));
}

template<typename E>
E* EntityMgr::onCreateEntity(PyObject* obj, IScriptDefModule* defModule){
	return NEW(obj) E(0, defModule);
}


PyObject* EntityMgr::__py__createBase(PyObject* self, PyObject* args){
	int argCount = (int)PyTuple_Size(args);
	PyObject* params = NULL;
	char* entityType = NULL;
	int ret = -1;

	if (argCount == 2)
		ret = PyArg_ParseTuple(args, "s|O", &entityType, &params);
	else
		ret = PyArg_ParseTuple(args, "s", &entityType);

	if (entityType == NULL || ret == -1)
	{
		PyErr_Format(PyExc_AssertionError, "Baseapp::createBase: args error!");
		PyErr_PrintEx(0);
		return NULL;
	}

	PyObject* e = (PyObject*)s_self->createBase(entityType, params);
	if (e != NULL)
		Py_INCREF(e);

	return e;
}

void EntityMgr::test(){
	PyRun_SimpleString("from ddddtest import test\ntest()\n");
}