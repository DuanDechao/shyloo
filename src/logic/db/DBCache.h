#ifndef __SL_CORE_DB_CACHE_H__
#define __SL_CORE_DB_CACHE_H__

class DBCache{
public:
	struct CacheInfo{
		CacheInfo():_object(NULL) {}
		IObject* _object;
		std::set<uint32> _dirtyProps;
		DBTable* _table;
	};

	DBCache(DataBase* dataBase): _dataBase(dataBase){}
	virtual ~DBCache() {}

	bool update(const int32 tableId, const uint64 dbid, sl::OBStream& data);
	bool read(const int32 tableId, const uint64 dbid, sl::IBStream& data);
	bool getDirtyData(const int32 tableId, const uint64 dbid, sl::IBStream& data);
	bool clearCache(const int32 tableId, const uint64 dbid);

protected:
	CacheInfo& getCacheInfo(const int32 tableId, const uint64 dbid);

private:
	typedef std::unordered_map<int32, std::unordered_map<uint64, CacheInfo>> CACHE_DATA;
	CACHE_DATA _allCacheData;
	DataBase*	_dataBase;
};
