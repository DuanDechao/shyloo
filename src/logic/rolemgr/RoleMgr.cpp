#include "RoleMgr.h"
#include "IHarbor.h"
#include "ICacheDB.h"
#include "Role.h"
#include "IDCCenter.h"

bool RoleMgr::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	return true;
}

bool RoleMgr::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_cacheDB, CacheDB);
	return true;
}
bool RoleMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

bool RoleMgr::getRoleList(int64 account, const std::function <void(sl::api::IKernel* pKernel, const int64 actorId, IRole* role)>& f){
	auto accItor = _accountMap.find(account);
	if (accItor == _accountMap.end()){
		_cacheDB->readByIndex("actor", [&](sl::api::IKernel* pKernel, ICacheDBReader* reader){
			reader->readColumn("id");
			reader->readColumn("name");
			reader->readColumn("occupation");
			reader->readColumn("sex");
		}, [&](sl::api::IKernel* pKernel, ICacheDBReadResult* result){
			int32 count = result->count();
			for (int32 i = 0; i < count; i++){
				int64 id = result->getInt64(i, 0);
				const char* name = result->getString(i, 1);
				int8 occupation = result->getInt8(i, 2);
				int8 sex = result->getInt8(i, 3);
				Role* newRole = NEW Role(account, id, name, occupation, sex);
				_roleMap.insert(make_pair(id, newRole));
				_accountMap[account].push_back(id);
				f(_kernel, id, newRole);
			}
		}, account);
	}
	else{
		for (auto actorId : _accountMap[account]){
			SLASSERT(_roleMap.find(actorId) != _roleMap.end(), "wtf");
			f(_kernel, actorId, _roleMap[actorId]);
		}
	}
	return true;
}

IRole* RoleMgr::createRole(int64 accountId, int64 actorId, const sl::OBStream& buf){
	const char* name = nullptr;
	int8 occupation = 0;
	int8 sex = 0;
	if (!buf.read(name) || !buf.read(occupation) || !buf.read(sex)){
		SLASSERT(false, "wtf");
		return nullptr;
	}

	bool ret = _cacheDB->writeByIndex("actor", [&](sl::api::IKernel* pKernel, ICacheDBContext* context){
		context->writeInt64("id", actorId);
		context->writeString("name", name);
		context->writeInt8("occupation", occupation);
		context->writeInt8("sex", sex);
	}, accountId);

	if (!ret)
		return nullptr;
	
	Role* newRole = NEW Role(accountId, actorId, name, occupation, sex);
	_roleMap.insert(make_pair(actorId, newRole));
	_accountMap[accountId].push_back(actorId);
	return newRole;
}

bool RoleMgr::loadRole(const int64 actorId, IObject* object){
	_cacheDB->read("actor", [&](sl::api::IKernel* pKernel, ICacheDBReader* reader){
		reader->readColumn("id");
		reader->readColumn("name");
		reader->readColumn("occupation");
		reader->readColumn("sex");
	}, [&object](sl::api::IKernel* pKernel, ICacheDBReadResult* result){
		int32 count = result->count();
		if (count != 1){
			SLASSERT(false, "invailed data size");
		}
		else{
			int64 id = result->getInt64(0, 0);
			const char* name = result->getString(0, 1);
			int8 occupation = result->getInt8(0, 2);
			int8 sex = result->getInt8(0, 3);
			object->setPropInt64(attr_def::id, id);
			object->setPropString(attr_def::name, name);
			object->setPropInt8(attr_def::occupation, occupation);
			object->setPropInt8(attr_def::sex, sex);
		}
	}, 1, actorId);
	return true;
}

void RoleMgr::recoverPlayer(IObject* player){

}