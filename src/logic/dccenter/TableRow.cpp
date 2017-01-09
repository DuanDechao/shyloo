#include "TableRow.h"

TableRow::TableRow(TableControl* pTable,TableColumn* pTableCol)
	:m_pTable(pTable), 
	m_pTableColumn(pTableCol)
{
	m_pRowData = NEW OMemory(m_pTableColumn->getMemSize());
}

TableRow::~TableRow(){
	DEL m_pRowData;
	m_pRowData = nullptr;
	m_pTable = nullptr;
	m_pTableColumn = nullptr;
}

const void* TableRow::getData(const int32 col, const int8 type, int32 & size) const{
	const TableLayout* info = m_pTableColumn->query(col, type, size);
	if (!info){
		SLASSERT(false, "have invaild Column Info");
		return nullptr;
	}
	size = info->_size;
	return m_pRowData->getData(info);
}

void TableRow::setData(const int32 col, const int8 type, const void* pszBuf, const int32 size, bool changeKey){
	const TableLayout* info = m_pTableColumn->query(col, type, size);
	if (!info)
		return;

	m_pRowData->setData(info, pszBuf, size);
}