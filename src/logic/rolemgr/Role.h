#ifndef __SL_FRAMEWORK_ROLE_H__
#define __SL_FRAMEWORK_ROLE_H__
#include "IRoleMgr.h"
#include "slstring.h"
#include "GameDefine.h"
class Role :public IRole{
public:
	Role(int64 accountId, int64 actorId, const char* name, int8 occupation, int8 sex)
		:_accountId(accountId),
		_actorId(actorId),
		_name(name),
		_occupation(occupation),
		_sex(sex)
	{}

	virtual void pack() {}

	virtual int64 getRoleId() { return _actorId; }

private:
	int64 _accountId;
	int64 _actorId;
	sl::SLString<game::MAX_ROLE_NAME_LEN> _name;
	int8 _occupation;
	int8 _sex;
};

#endif