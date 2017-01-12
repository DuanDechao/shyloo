#ifndef __SL_TABLE_CONTROL_H__
#define __SL_TABLE_CONTROL_H__
#include <vector>
#include <unordered_map>
#include "sltype.h"
#include "slstring.h"
#include "IDCCenter.h"
#define MAX_STRING_KEY_LEN 64

class TableRow;
class TableColumn;
class IObject;
class IRow;
class TableControl : public ITabelControl{
	typedef std::vector<TableRow *> TABLE_ROWS;
	typedef std::unordered_map<int64, int32> KEY_INT_MAP;
	typedef std::unordered_map<sl::SLString<MAX_STRING_KEY_LEN>, int32, sl::HashFunc<MAX_STRING_KEY_LEN>, sl::EqualFunc<MAX_STRING_KEY_LEN>> KEY_STRING_MAP;

public:
	TableControl(int32 name, TableColumn* pTableColumn, IObject* pHost = nullptr);
	~TableControl();

	int32 getName() const { return m_name; }
	virtual IObject* getHost() const { return m_pHost; }
	virtual int32 rowCount() const { return (int32)m_tableRows.size(); }
	virtual void clearRows();
	virtual const IRow* findRow(const int64 key) const;
	virtual const IRow* findRow(const char* key) const;
	const IRow* getRow(const int32 index) const;

	virtual IRow* addRow();
	virtual IRow* addRowKeyInt8(const int8 key) { return addRowKey(DTYPE_INT8, &key, sizeof(key), key); }
	virtual IRow* addRowKeyInt16(const int16 key) { return addRowKey(DTYPE_INT16, &key, sizeof(key), key); }
	virtual IRow* addRowKeyInt32(const int32 key) { return addRowKey(DTYPE_INT32, &key, sizeof(key), key); }
	virtual IRow* addRowKeyInt64(const int64 key) { return addRowKey(DTYPE_INT64, &key, sizeof(key), key); }
	virtual IRow* addRowKeyString(const char* key);
	IRow* addRowKey(const int8 type, const void * data, const int32 size, const int64 key);

	virtual bool delRow(const int32 index);

	void changeKey(const int64 newKey, const int64 oldKey, const int8 type);
	void changeKey(const char* newKey, const char* oldKey, const int8 type);

	void updateRowKeyIndex(const int32 index);
private:
	int32			m_name;
	TableColumn*	m_pTableColumn;
	IObject*		m_pHost;
	TABLE_ROWS		m_tableRows;
	KEY_INT_MAP		m_keyToColIdx;
	KEY_STRING_MAP  m_strToColIdx;

	
};
#endif