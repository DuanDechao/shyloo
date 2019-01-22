#ifndef SL_GLOBAL_DATA_H
#define	SL_GLOBAL_DATA_H
#include "slmulti_sys.h"
#include <list>
#include "IGlobalData.h"
#include <set>
namespace GLOBAL_DATA {
	static const char* TABLE_NAME = "GLOBAL_DATA";
	enum COLUMN{
		OCTM_START = 0,
		KEY = OCTM_START,
		TYPE,
		DATA_SIZE,
		DATA,
		OCTM_END,
	};
}

class ITableControl;
class GlobalData{
public:
	GlobalData();
	~GlobalData();

	bool write(const char* key, const int16 dataType, const void* data, const int32 dataSize);
	bool del(const char* key);
	void addListener(IGlobalDataListener* listener);
	const void* getData(const char* key, int16& dataType, int32& dataSize);
	bool hasData(const char* key);
	inline std::set<std::string>& getKeys() {return _keys;}

private:
	void initGlobalDataTable();
	void onDataChanged(const char* key, const int16 dataType, const void* data, const int32 dataSize, bool isDelete = false);

private:
	ITableControl* _data;
	std::list<IGlobalDataListener*> _listeners;
	std::set<std::string> _keys;
};
#endif
