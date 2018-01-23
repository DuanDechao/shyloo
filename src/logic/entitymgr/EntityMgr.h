#ifndef SL_LOGIC_ENTITY_MGR_H
#define SL_LOGIC_ENTITY_MGR_H
#include "slikernel.h"
#include "IEntityMgr.h"
#include "IEntityDef.h"
#include <unordered_map>

class ScriptObject;
class Base;
class Entity;
class Proxy;
class EntityMgr : public IEntityMgr, public sl::api::ITimer{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	 Base* createBase(const char* entityType, PyObject* params, bool initializeScript = true, const uint64 entityId = 0, bool initProperty = true);
     Entity* createCellEntity(const char* entityType, PyObject* params, bool initializeScript = true, const uint64 entityId = 0, bool initProperty = true);
     Proxy* createProxy(const char* entityType, PyObject* params, bool initializeScript = true, const uint64 entityId = 0, bool initProperty = true);
	
    virtual bool createBaseFromDB(const char* entityType, const uint64 dbid, PyObject* pyCallback);

	static PyObject* __py__createBase(PyObject* self, PyObject* args);
	static PyObject* __py__createBaseLocallyFromDB(PyObject* self, PyObject* args);
    
    
	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick){}


    void onRemoteNewEntityMail(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);
    void onCellEntityCreatedFromCell(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);
    void onCreateCellEntityOnCell(sl::api::IKernel* pKernel, const void* context, const int32 size);
    
    void onBaseEventCellEntityCreated(sl::api::IKernel* pKernel, const void* context, const int32 size);

    void onNewPlayerLogined(sl::api::IKernel* pKernel, const void* context, const int32 size);
    void onEntityCreatedFromDB(sl::api::IKernel* pKernel, const void* context, const int32 size);
	void test();

    static inline uint64 allocCallbackId(){
        static uint64 id = 0;
        return ++id;
    }

private:
    template<typename E>
    E* createEntity(const char* entityType, PyObject* params, bool isInitializeScript, const uint64 entityId, bool initProperty);

	template<typename E>
	E* onCreateEntity(const uint64 entityId, PyObject* obj, IScriptDefModule* defModule);
    

private:
	static EntityMgr*		s_self;
	sl::api::IKernel*		_kernel;
    std::unordered_map<uint64, ScriptObject*> _entities;
    std::unordered_map<uint64, PyObject*> _pyCallbacks;
};
#endif
