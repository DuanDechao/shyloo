#include "RoleMgr.h"
#include "IHarbor.h"
#include "ICacheDB.h"
#include "Role.h"
#include "IDCCenter.h"
#include "slbinary_map.h"
#include "IIdMgr.h"

bool RoleMgr::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	return true;
}

bool RoleMgr::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_cacheDB, CacheDB);
	FIND_MODULE(_idMgr, IdMgr);

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

IRole* RoleMgr::createRole(int64 accountId, const sl::OBStream& buf){
	const char* name = nullptr;
	int8 occupation = 0;
	int8 sex = 0;
	if (!buf.readString(name) || !buf.readInt8(occupation) || !buf.readInt8(sex)){
		SLASSERT(false, "wtf");
		return nullptr;
	}
	int64 actorId = _idMgr->allocID();
	bool ret = _cacheDB->write("actor", true, [&](sl::api::IKernel* pKernel, ICacheDBContext* context){
		context->writeInt64("account", accountId);
		context->writeString("name", name);
		context->writeInt8("occupation", occupation);
		context->writeInt8("sex", sex);
		context->writeInt8("firstLogin", 1);
		context->writeBlob("props", nullptr, 0);
	}, 1, actorId);

	if (!ret)
		return nullptr;

	ECHO_TRACE("client[%lld] create role[%s:%lld] success", accountId, name, actorId);
	
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
		reader->readColumn("props");
		reader->readColumn("scene");
		reader->readColumn("x");
		reader->readColumn("y");
		reader->readColumn("z");
		reader->readColumn("firstLogin");
	}, [&object](sl::api::IKernel* pKernel, ICacheDBReadResult* result){
		int32 count = result->count();
		if (count != 1){
			SLASSERT(false, "invailed data size");
		}
		else{
			object->setPropInt64(attr_def::id, result->getInt64(0, 0));
			object->setPropString(attr_def::name, result->getString(0, 1));
			object->setPropInt8(attr_def::occupation, result->getInt8(0, 2));
			object->setPropInt8(attr_def::sex, result->getInt8(0, 3));
			object->setPropString(attr_def::scene, result->getString(0, 5));
			object->setPropFloat(attr_def::x, result->getFloat(0, 6));
			object->setPropFloat(attr_def::y, result->getFloat(0, 7));
			object->setPropFloat(attr_def::z, result->getFloat(0, 8));
			object->setPropInt8(attr_def::firstLogin, result->getInt8(0, 9));

			int32 size = 0;
			const void* propsData = result->getBinary(0, 4, size);
			if (propsData){
				sl::OBMap args(propsData, size);
				for (const IProp* prop : object->getObjProps()){
					int32 setting = prop->getSetting(object);
					//if ((setting & prop_def::save) && (setting & prop_def::blob)){
						switch (prop->getType(object)){
						case DTYPE_INT8: object->setPropInt8(prop, args.getInt8(prop->getName())); break;
						case DTYPE_INT16: object->setPropInt16(prop, args.getInt16(prop->getName())); break;
						case DTYPE_INT32: object->setPropInt32(prop, args.getInt32(prop->getName())); break;
						case DTYPE_INT64: object->setPropInt64(prop, args.getInt64(prop->getName())); break;
						case DTYPE_FLOAT: object->setPropFloat(prop, args.getFloat(prop->getName())); break;
						case DTYPE_STRING: object->setPropString(prop, args.getString(prop->getName())); break;
						case DTYPE_BLOB: {
								int32 size = 0;
								const void* p = args.getBlob(prop->getName(), size);
								if (p)
									object->setPropBlob(prop, p, size);

							} 
							break;
						}
				//	}
				}
			}
		}
	}, 1, actorId);
	return true;
}

void RoleMgr::recoverPlayer(IObject* player){

}
