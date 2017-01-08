#ifndef SL_LOGIC_OBJECT_MGR_H
#define SL_LOGIC_OBJECT_MGR_H
#include "IDCCenter.h"
#include "slstring.h"
#include "ObjectStruct.h"
#include <unordered_map>

class ObjectMgr :public IObjectMgr
{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	bool initPropDefineConfig(sl::api::IKernel * pKernel);
	bool loadObjectPropConfig(sl::api::IKernel * pKernel);
	
	ObjectPropInfo* loadObjectProp(const char* objectName);

private:
	typedef std::unordered_map<sl::SLString<game::MAX_PROP_NAME_LEN>, int32, sl::HashFunc<game::MAX_PROP_NAME_LEN>, sl::EqualFunc<game::MAX_PROP_NAME_LEN>> PROP_DEFINE_MAP;
	typedef std::unordered_map<sl::SLString<game::MAX_PROP_NAME_LEN>, ObjectPropInfo, sl::HashFunc<game::MAX_PROP_NAME_LEN>, sl::EqualFunc<game::MAX_PROP_NAME_LEN>> PROP_CONFIG_MAP;
	typedef std::unordered_map<sl::SLString<game::MAX_PROP_NAME_LEN>, sl::SLString<game::MAX_PATH_LEN>, sl::HashFunc<game::MAX_PROP_NAME_LEN>, sl::EqualFunc<game::MAX_PROP_NAME_LEN>> PROP_CONFIG_PATH_MAP;
	static PROP_DEFINE_MAP s_propDefine;
	static PROP_CONFIG_MAP s_propConfigs;
	static PROP_CONFIG_PATH_MAP s_propConfigsPath;

};
#endif