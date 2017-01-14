#include "TableRow.h"
#include "TableControl.h"

bool TableColumn::loadColumnConfig(const sl::ISLXmlNode& root){
	const sl::ISLXmlNode& columns = root["column"];
	for (int32 i = 0; i < columns.count(); i++){
		const char* type = columns[i].getAttributeString("type");
		bool isKey = columns[i].getAttributeBoolean("key");

		if (!strcmp(type, "int8")){
			addColumn(DTYPE_INT8, m_size, sizeof(int8), isKey);
		}
		else if (!strcmp(type, "int16")){
			addColumn(DTYPE_INT16, m_size, sizeof(int16), isKey);
		}
		else if (!strcmp(type, "int32")){
			addColumn(DTYPE_INT32, m_size, sizeof(int32), isKey);
		}
		else if (!strcmp(type, "int64")){
			addColumn(DTYPE_INT64, m_size, sizeof(int64), isKey);
		}
		else if (!strcmp(type, "float")){
			addColumn(DTYPE_FLOAT, m_size, sizeof(float), isKey);
		}
		else if (!strcmp(type, "string")){
			int32 size = columns[i].getAttributeInt32("size");
			addColumn(DTYPE_STRING, m_size, size, isKey);
		}
		else{
			SLASSERT(false, "invalid type");
			return false;
		}
	}
	return true;
}

TableRow::TableRow(TableControl* pTable,const TableColumn* pTableCol)
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

	if (changeKey && info->_isKey){
		switch (info->_type){
		case DTYPE_INT8: m_pTable->changeKey(*(int8*)pszBuf, *(int8*)m_pRowData->getData(info), info->_type); break;
		case DTYPE_INT16: m_pTable->changeKey(*(int16*)pszBuf, *(int16*)m_pRowData->getData(info), info->_type); break;
		case DTYPE_INT32: m_pTable->changeKey(*(int32*)pszBuf, *(int32*)m_pRowData->getData(info), info->_type); break;
		case DTYPE_INT64: m_pTable->changeKey(*(int64*)pszBuf, *(int64*)m_pRowData->getData(info), info->_type); break;
		case DTYPE_STRING: m_pTable->changeKey((const char*)pszBuf, (const char*)m_pRowData->getData(info), info->_type); break;
		default: SLASSERT(false, "invaild key type"); break;
		}
	}
	m_pRowData->setData(info, pszBuf, size);
}