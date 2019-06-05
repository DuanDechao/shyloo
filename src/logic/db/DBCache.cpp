#include "DBCache.h"
bool DBCache::update(const int32 tableId, const uint64 dbid, sl::OBStream& data){
	CacheInfo& cache = getCacheInfo(tableId, dbid);
	DBTable* pTable = cache._table;
	pTable->
	
}

CacheInfo& DBCache::getCacheInfo(const int32 tableId, const uint64 dbid){
	CacheInfo& cache = _allCacheData[tableId][dbid];
	if(cache._object == NULL){
		DBTable* pTable = _dataBase->findTable(tableId);
		SLASSERT(pTable, "wtf");
		cache._table = pTable;
		cache._object = CREATE_OBJECT_BYID(pTable->tableName, dbid);
		SLASSERT(cache._object, "wtf");
	}
	return cache;
}
