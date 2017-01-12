#ifndef SL_LOGIC_OBJECT_MGR_H
#define SL_LOGIC_OBJECT_MGR_H
#include "IDCCenter.h"
#include "slstring.h"
#include "ObjectStruct.h"
#include <unordered_map>

class ObjectProp;
class ObjectMgr :public IObjectMgr
{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	bool initPropDefineConfig(sl::api::IKernel * pKernel);
	bool loadObjectPropConfig(sl::api::IKernel * pKernel);
	
	static const IProp* setObjectProp(const char* propName, const int32 objTypeId, PropLayout* layout);

private:
	ObjectPropInfo* createTemplate(sl::api::IKernel* pKernel, const char* objectName);
	ObjectPropInfo* queryTemplate(sl::api::IKernel* pKernel, const char* objectName);

private:
	typedef std::unordered_map<sl::SLString<MAX_PROP_NAME_LEN>, int32, sl::HashFunc<MAX_PROP_NAME_LEN>, sl::EqualFunc<MAX_PROP_NAME_LEN>> PROP_DEFINE_MAP;
	typedef std::unordered_map<sl::SLString<MAX_OBJECT_NAME_LEN>, sl::SLString<game::MAX_PATH_LEN>, sl::HashFunc<MAX_OBJECT_NAME_LEN>, sl::EqualFunc<MAX_OBJECT_NAME_LEN>> PROP_CONFIG_PATH_MAP;
	typedef std::unordered_map<sl::SLString<MAX_PROP_NAME_LEN>, ObjectProp*, sl::HashFunc<MAX_PROP_NAME_LEN>, sl::EqualFunc<MAX_PROP_NAME_LEN>> PROP_MAP;
	typedef std::unordered_map<sl::SLString<MAX_OBJECT_NAME_LEN>, ObjectPropInfo *, sl::HashFunc<MAX_OBJECT_NAME_LEN>, sl::EqualFunc<MAX_OBJECT_NAME_LEN>> OBJECT_MODEL_MAP;
	static PROP_DEFINE_MAP s_propDefine;
	static PROP_CONFIG_PATH_MAP s_propConfigsPath;
	static PROP_MAP s_allProps;
	static OBJECT_MODEL_MAP s_objPropInfo;
	static int32 s_nextObjTypeId;


};
#endif