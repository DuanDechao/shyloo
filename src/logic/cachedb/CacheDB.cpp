#include "CacheDB.h"
#include "slxml_reader.h"
#include "slargs.h"
#include "CacheDBStruct.h"
#include "IRedis.h"

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

			if (!strcmp(typeStr, "int8")){
				tableDesc.columns[name] = CDB_TYPE_INT8;
			}
			else if (!strcmp(typeStr, "int16")){
				tableDesc.columns[name] = CDB_TYPE_INT16;
			}
			else if (!strcmp(typeStr, "int32")){
				tableDesc.columns[name] = CDB_TYPE_INT32;
			}
			else if (!strcmp(typeStr, "int64")){
				tableDesc.columns[name] = CDB_TYPE_INT64;
			}
			else if (!strcmp(typeStr, "float")){
				tableDesc.columns[name] = CDB_TYPE_FLOAT;
			}
			else if (!strcmp(typeStr, "string")){
				tableDesc.columns[name] = CDB_TYPE_STRING;
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
		if (tableDesc.key == ""){
			SLASSERT(false, "etyf");
			return false;
		}
		if (tables[i].subNodeExist("index")){
			const char* index = tables[i]["index"][0].getAttributeString("name");
			auto itor = tableDesc.columns.find(index);
			if (itor != tableDesc.columns.end()){
				tableDesc.index = { itor->second, itor->first };
			}
			else{
				SLASSERT(false, "eyf");
				return false;
			}
		}
		_tables[tableDesc.name] = tableDesc;
	}

	return true;
}
bool CacheDB::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_redis, Redis);
	test();
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
	if (_tables.find(table) == _tables.end())
		return false;

	CacheTable& desc = _tables[table];


	return true;
}

bool CacheDB::readByIndex(const char* table, const CacheDBColumnFuncType& cf, const CacheDBReadFuncType& f, const char* index){
	return true;
}

bool CacheDB::write(const char* table, const CacheDBWriteFuncType& f, int32 count, ...){
	if (count > 0 && _tables.find(table) != _tables.end()){
		CacheTable& desc = _tables[table];
		IArgs<MAX_KEYS, MAX_ARGS> args;

		CacheDBContext context(desc, args);
		f(_kernel, &context);

		va_list ap;
		va_start(ap, count);
		int8 type = desc.columns[desc.key];
		char temp[128];
		SafeSprintf(temp, 128, "%s|", table);
		args << temp << desc.key.c_str() << count;
		for (int32 i = 0; i < count; i++){
			switch (type){
			case CDB_TYPE_INT8: args << (int8)va_arg(ap, int8); break;
			case CDB_TYPE_INT16: args << (int16)va_arg(ap, int16); break;
			case CDB_TYPE_INT32: args << (int32)va_arg(ap, int32); break;
			case CDB_TYPE_INT64: args << (int64)va_arg(ap, int64); break;
			case CDB_TYPE_STRING: args << va_arg(ap, const char*); break;
			default: SLASSERT(false, "wtf");
			}
		}
		va_end(ap);

		if (context.isChangedIndex()){
			char tmp[128];
			SafeSprintf(tmp, sizeof(tmp), "%s|i+", table);
			args << tmp << desc.index.name.c_str();
		}

		args.fix();
		return _redis->call(0, "db_set", context.count() * 2, args.out());
	}
	return false;
}

bool CacheDB::writeByIndex(const char* table, const CacheDBWriteFuncType& f, const int64 index){
	if (_tables.find(table) == _tables.end())
		return false;
	
	CacheTable& desc = _tables[table];

	IArgs<MAX_KEYS, MAX_ARGS> args;

	CacheDBContext context(desc, args);
	f(_kernel, &context);

	char temp[128];
	SafeSprintf(temp, 128, "%s|i+%lld", table, index);
	args << temp;

	args.fix();

	return _redis->call(0, "db_set_index", context.count() * 2, args.out());

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

void CacheDB::test(){
	/*write("actor", [&](sl::api::IKernel* pKernel, ICacheDBContext* context){
		context->writeInt64("account", 1454565);
		context->writeString("name", "fyyyy");
		}, 1, (int64)23432);*/

	writeByIndex("actor", [&](sl::api::IKernel* pKernel, ICacheDBContext* context){
		context->writeString("name", "ddc");
	}, (int64)1454565);
}