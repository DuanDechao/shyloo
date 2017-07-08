#ifndef __SL_TABLE_ROW_H__
#define __SL_TABLE_ROW_H__
#include "Memory.h"
#include "IDCCenter.h"
#include "slxml_reader.h"
#include <vector>
struct TableLayout : public MemLayout{
	int8 _type;
	bool _isKey;
};

class TableColumn{
public:
	TableColumn() :m_keyType(DTYPE_CANT_BE_KEY), m_keyColIdx(-1), m_size(0){}
	~TableColumn(){}

	bool loadColumnConfig(const sl::ISLXmlNode& root);

	inline int8 getKeyType() const { return m_keyType; }
	inline int32 getKeyColumn() const { return m_keyColIdx; }
	inline int32 getMemSize() const { return m_size; }

	const TableLayout* query(const int32 col, const int8 type, const int32 size) const{
		if (col < 0 || col >= (int32)m_columns.size()){
			SLASSERT(false, "invalid params col idx");
			return nullptr;
		}
		const TableLayout& layout = m_columns[col];
		if (layout._type != type || layout._size < size){
			SLASSERT(false, "invaild params type or size");
			return nullptr;
		}
		return &layout;
	}

private:
	void addColumn(const int8 type, int32 offset, int32 size, bool isKey){
		TableLayout layout;
		layout._offset = offset;
		layout._size = size;
		layout._type = type;
		layout._isKey = isKey;

		if (isKey){
			SLASSERT(m_keyType == DTYPE_CANT_BE_KEY, "key has been set");
			m_keyType = type;
			m_keyColIdx = (int32)m_columns.size();
		}
		m_columns.push_back(layout);
		m_size += size;
	}
private:
	std::vector<TableLayout> m_columns;
	int8 m_keyType;
	int32 m_keyColIdx;
	int32 m_size;
};

class TableControl;
class TableRow : public IRow{
public:
	TableRow(TableControl* pTable, const TableColumn* pTableCol);
	virtual ~TableRow();

	int32 getRowIndex() const { return _rowIndex; }
	void setRowIndex(const int32 index) { _rowIndex = index; }

	virtual int8 getDataInt8(const int32 col) const { int32 size = sizeof(int8); return *(int8*)getData(col, DTYPE_INT8, size); }
	virtual int16 getDataInt16(const int32 col) const{ int32 size = sizeof(int16); return *(int16*)getData(col, DTYPE_INT16, size); }
	virtual int32 getDataInt32(const int32 col) const{ int32 size = sizeof(int32); return *(int32*)getData(col, DTYPE_INT32, size); }
	virtual int64 getDataInt64(const int32 col) const{ int32 size = sizeof(int64); return *(int64*)getData(col, DTYPE_INT64, size); }
	virtual float getDataFloat(const int32 col) const{ int32 size = sizeof(float); return *(float*)getData(col, DTYPE_FLOAT, size); }
	virtual const char* getDataString(const int32 col) const{ int32 size = 0; return (const char *)getData(col, DTYPE_STRING, size); }

	virtual void setDataInt8(const int32 col, const int8 val) const { setData(col, DTYPE_INT8, &val, sizeof(int8)); }
	virtual void setDataInt16(const int32 col, const int16 val) const { setData(col, DTYPE_INT16, &val, sizeof(int16)); }
	virtual void setDataInt32(const int32 col, const int32 val) const { setData(col, DTYPE_INT32, &val, sizeof(int32)); }
	virtual void setDataInt64(const int32 col, const int64 val) const{ setData(col, DTYPE_INT64, &val, sizeof(int64)); }
	virtual void setDataFloat(const int32 col, const float val) const { setData(col, DTYPE_FLOAT, &val, sizeof(float)); }
	virtual void setDataString(const int32 col, const char* val)const { setData(col, DTYPE_STRING, val, (int32)(strlen(val) + 1)); }

	const void* getData(const int32 col, const int8 type, int32 & size) const;
	void setData(const int32 col, const int8 tpye, const void* pszBuf, const int32 size, bool changeKey = true) const;

private:
	OMemory*			_pRowData;
	TableControl*		_pTable;
	const TableColumn*	_pTableColumn;

	int32				_rowIndex;

};
#endif
