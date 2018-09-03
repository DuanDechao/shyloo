#ifndef SL_LOGIC_PYTHON_SERVER_H
#define SL_LOGIC_PYTHON_SERVER_H
#include "slikernel.h"
#include "IPythonServer.h"
#include "IObjectDef.h"
#include <unordered_map>
#include "EntityScriptObject.h"
#include "Entities.h"
#include "IMonitor.h"
#include "slsingleton.h"
#include "GlobalData.h"
#include "pyscript/pyscript.h"
class Base;
class Entity;
class Proxy;
class ScriptDefModule;
class PythonServer : public IPythonServer, public IMonitorListener, public sl::api::ITimer, public sl::SLHolder<PythonServer>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);
	
	virtual IDataType* createPyDataTypeFromXml(const char* typeName, const sl::ISLXmlNode* node); 
	virtual IDataType* createPyDataType(const char* typeName);
	virtual void onEntityCreatedFromDB(IObject* object, const int32 callbackId, bool wasActive);
	virtual void onBaseCreateAnywhere(IObject* object,  PyObject* params, const int32 callbackId);

	Base* createBase(IObject* object, PyObject* params, bool initializeScript = true);
    Entity* createCellEntity(IObject* object, PyObject* params, bool initializeScript = true);
    Proxy* createProxy(IObject* object, PyObject* params, bool initializeScript = true);

    virtual bool createBaseFromDB(const char* entityType, const uint64 dbid, PyObject* pyCallback);
	virtual bool createBaseAnywhere(const char* entityType, PyObject* params, PyObject* pyCallback);

	static PyObject* __py__createBase(PyObject* self, PyObject* args);
	static PyObject* __py__createBaseLocallyFromDB(PyObject* self, PyObject* args);
	static PyObject* __py__genUUID64(PyObject* self, PyObject* args);
	static PyObject* __py__createBaseAnywhere(PyObject* self, PyObject* args);
	static PyObject* __py__hasRes(PyObject* self, PyObject* args);
	static PyObject* __py__addSpaceGeometryMapping(PyObject* self, PyObject* args);
	static PyObject* __py__createEntity(PyObject* self, PyObject* args);
	static PyObject* __py__scriptLogType(PyObject* self, PyObject* args);
	static PyObject* __py__publish(PyObject* self, PyObject* args);
	static PyObject* __py__addWatcher(PyObject* self, PyObject* args);
	static PyObject* __py__getResFullPath(PyObject* self, PyObject* args);
    
	virtual void onInit(sl::api::IKernel* pKernel, int64 timetick){ printf("OnInit------------- %lld\n", sl::getTimeMilliSecond());}
	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){ test();}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick){ printf("OnTime------------------%lld\n", sl::getTimeMilliSecond());}
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick){}
	
	virtual bool onServerReady(sl::api::IKernel* pKernel);
	virtual bool onServerReadyForLogin(sl::api::IKernel* pKernel);
	virtual bool onServerReadyForShutDown(sl::api::IKernel* pKernel);
	virtual bool onServerShutDown(sl::api::IKernel* pKernel);

    //void onCellEntityCreatedFromCell(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);
    
	void onCreateCellEntityOnCell(sl::api::IKernel* pKernel, const void* context, const int32 size);
    void onCellEntityCreatedFromCell(sl::api::IKernel* pKernel, const void* context, const int32 size);
    void onProxyCreated(sl::api::IKernel* pKernel, const void* context, const int32 size);
    void onCellEntityCreatedOnCell(sl::api::IKernel* pKernel, const void* context, const int32 size);
    void onCreateBaseAnywhere(sl::api::IKernel* pKernel, const void* context, const int32 size);
    void onCreateBaseAnywhereCallback(sl::api::IKernel* pKernel, const void* context, const int32 size);
	void onRemoteMethodCall(sl::api::IKernel* pKernel, const void* context, const int32 size);
    //void onEntityCreatedFromDB(sl::api::IKernel* pKernel, const void* context, const int32 size);
	void test();

	sl::api::IKernel* getKernel() const { return _kernel;}
	
	ScriptDefModule* findScriptDefModule(const char* entityType);
	ScriptDefModule* findScriptDefModule(const int32 entityType);

	inline EntityScriptObject* findEntity(uint64 objectId) {return _entities->find(objectId);}

    static inline uint64 allocCallbackId(){
        static uint64 id = 0;
        return ++id;
    }

	inline const IProp* getPropPyObject() const {return _propPyObject;}

private:
    template<typename E>
    E* createEntity(IObject* object, PyObject* params, bool isInitializeScript);

	template<typename E>
	E* onCreateEntity(IObject* object, PyObject* obj, ScriptDefModule* defModule);

	void rgsBaseScriptModule(PyTypeObject* type);
	bool loadAllScriptModules(sl::api::IKernel* pKernel, std::vector<PyTypeObject*>& scriptBaseTypes);
	bool isLoadScriptModule(const IObjectDefModule* defModule);
	bool loadEntryScript(sl::api::IKernel* pKernel);
	inline PyScript& getScript() {return _pyScript;} 

private:
	static PythonServer*								s_self;
	sl::api::IKernel*									_kernel;
	PyScript											_pyScript;
	std::unordered_map<std::string, PyMethodDef>		_scriptMethods;

	Entities*											_entities;
    std::unordered_map<uint64, PyObject*>				_pyCallbacks;
    std::unordered_map<std::string, ScriptDefModule*>	_scriptDefModules;
    std::unordered_map<int32, ScriptDefModule*>			_typeToScriptDefModules;
	std::vector<PyTypeObject*>							_scriptBaseTypes;
	PyObject*											_entryScript;
	GlobalData*											_globalData;
	const IProp*										_propPyObject;
};
#endif
