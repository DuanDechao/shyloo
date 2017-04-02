#ifndef SL_LOGIC_OBJECT_MGR_H
#define SL_LOGIC_OBJECT_MGR_H
#include "IDCCenter.h"
#include "slstring.h"
#include "ObjectStruct.h"
#include <unordered_map>
#include "slsingleton.h"

class MMObject;
class ObjectProp;
class IIdMgr;
class ObjectMgr :public IObjectMgr, public sl::api::ITimer, public sl::SLHolder<ObjectMgr>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual const IProp* getPropByName(const char* name) const;
	virtual IObject* create(const char* file, const int32 line, const char* name);
	virtual IObject* createById(const char* file, const int32 line, const char* name, const uint64 id);
	virtual void recover(IObject* object);
	virtual IObject* findObject(const uint64 id);
	
	virtual ITableControl* createStaticTable(const char* name);
	virtual void recoverStaticTable(ITableControl* table);
	
	const IProp* setObjectProp(const char* propName, const int32 objTypeId, PropLayout* layout);

	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}

	const IProp* getObjectStatusProp(){ return _objectStatus; }

private:
	bool initPropDefineConfig(sl::api::IKernel * pKernel);
	bool loadObjectPropConfig(sl::api::IKernel * pKernel);

	ObjectPropInfo* createTemplate(sl::api::IKernel* pKernel, const char* objectName);
	ObjectPropInfo* queryTemplate(sl::api::IKernel* pKernel, const char* objectName);

private:
	typedef std::unordered_map<sl::SLString<MAX_PROP_NAME_LEN>, int32, sl::HashFunc<MAX_PROP_NAME_LEN>, sl::EqualFunc<MAX_PROP_NAME_LEN>> PROP_DEFINE_MAP;
	typedef std::unordered_map<sl::SLString<MAX_OBJECT_NAME_LEN>, sl::SLString<game::MAX_PATH_LEN>, sl::HashFunc<MAX_OBJECT_NAME_LEN>, sl::EqualFunc<MAX_OBJECT_NAME_LEN>> PROP_CONFIG_PATH_MAP;
	typedef std::unordered_map<sl::SLString<MAX_PROP_NAME_LEN>, ObjectProp*, sl::HashFunc<MAX_PROP_NAME_LEN>, sl::EqualFunc<MAX_PROP_NAME_LEN>> PROP_MAP;
	typedef std::unordered_map<sl::SLString<MAX_OBJECT_NAME_LEN>, ObjectPropInfo *, sl::HashFunc<MAX_OBJECT_NAME_LEN>, sl::EqualFunc<MAX_OBJECT_NAME_LEN>> OBJECT_MODEL_MAP;
	
	ObjectMgr* _self;
	IIdMgr* _idMgr;
	const IProp* _objectStatus;

	PROP_DEFINE_MAP _propDefine;
	PROP_CONFIG_PATH_MAP _propConfigsPath;
	PROP_MAP _allProps;
	OBJECT_MODEL_MAP _objPropInfo;
	int32 _nextObjTypeId;
	unordered_map<uint64, MMObject*> _allObjects;
    unordered_map<int32, TableColumn*>  _tablesInfo;
	unordered_map<int32, TableControl*>  _allTables;

	

};
#endif