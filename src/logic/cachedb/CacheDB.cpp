#include "CacheDB.h"
#include "slxml_reader.h"
#include "slargs.h"

bool CacheDB::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;

	char path[256] = { 0 };
	SafeSprintf(path, sizeof(path), "%s/cache_db.xml", pKernel->getEnvirPath());
	sl::XmlReader conf;
	if (!conf.loadXml(path)){
		SLASSERT(false, "can not load file %s", path);
		return false;
	}

	const sl::ISLXmlNode& tables = conf.root()["table"];
	for (int32 i = 0; i < tables.count(); i++){
		CacheTable tableDesc;
		tableDesc.name = tables[i].getAttributeString("name");
		tableDesc.del = tables[i].getAttributeBoolean("del");
		const sl::ISLXmlNode& columns = tables[i]["column"];
		for (int32 j = 0; j < columns.count(); j++){
			const char* name = columns[j].getAttributeString("name");
			const char* typeStr = columns[j].getAttributeString("type");

			if (!strcmp(typeStr, "s8")){
				tableDesc.columns[name] = CDB_TYPE_INT8;
			}
			else if (!strcmp(typeStr, "s16")){
				tableDesc.columns[name] = CDB_TYPE_INT16;
			}
			else if (!strcmp(typeStr, "s32")){
				tableDesc.columns[name] = CDB_TYPE_INT32;
			}
			else if (!strcmp(typeStr, "s64")){
				tableDesc.columns[name] = CDB_TYPE_INT64;
			}
			else if (!strcmp(typeStr, "float")){
				tableDesc.columns[name] = CDB_TYPE_FLOAT;
			}
			else{
				SLASSERT(false, "invalid type");
				return false;
			}

			if (columns[j].hasAttribute("key") && columns[j].getAttributeBoolean("key")){
				if (tableDesc.key == "")
					tableDesc.key = name;
				else{
					SLASSERT(false, "wtf");
					return false;
				}
			}
		}
	}

	return true;
}
bool CacheDB::launched(sl::api::IKernel * pKernel){
	return true;
}
bool CacheDB::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

bool CacheDB::read(const char* table, const CacheDBColumnFuncType& cf, const CacheDBReadFuncType& f, int32 count, ...){
	return true;
}

bool CacheDB::readByIndex(const char* table, const CacheDBColumnFuncType& cf, const CacheDBReadFuncType& f, const int64 index){
	return true;
}

bool CacheDB::readByIndex(const char* table, const CacheDBColumnFuncType& cf, const CacheDBReadFuncType& f, const char* index){
	return true;
}

bool CacheDB::write(const char* table, const CacheDBWriteFuncType& f, int32 count, ...){
	return true;
}

bool CacheDB::writeByIndex(const char* table, const CacheDBWriteFuncType& f, const int64 index){
	return true;
}

bool CacheDB::writeByIndex(const char* table, const CacheDBWriteFuncType& f, const char* index){
	return true;
}

bool CacheDB::del(const char* table, int32 count, ...){
	return true;
}

bool CacheDB::delByIndex(const char* table, const int64 index){
	return true;
}

bool CacheDB::delByIndex(const char* table, const char* index){
	return true;
}