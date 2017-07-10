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
	:_pTable(pTable), 
	_pTableColumn(pTableCol)
{
	_pRowData = NEW OMemory(_pTableColumn->getMemSize());
}

TableRow::~TableRow(){
	if (_pRowData)
		DEL _pRowData;

	_pRowData = nullptr;
	_pTable = nullptr;
	_pTableColumn = nullptr;
}

const void* TableRow::getData(const int32 col, const int8 type, int32 & size) const{
	const TableLayout* info = _pTableColumn->query(col, type, size);
	if (!info){
		SLASSERT(false, "have invaild Column Info");
		return nullptr;
	}
	size = info->_size;
	return _pRowData->getData(info);
}

void TableRow::setData(const int32 col, const int8 type, const void* pszBuf, const int32 size, bool changeKey) const{
	const TableLayout* info = _pTableColumn->query(col, type, size);
	if (!info)
		return;

	if (changeKey && info->_isKey){
		switch (info->_type){
		case DTYPE_INT8: _pTable->changeKey(*(int8*)pszBuf, *(int8*)_pRowData->getData(info), info->_type); break;
		case DTYPE_INT16: _pTable->changeKey(*(int16*)pszBuf, *(int16*)_pRowData->getData(info), info->_type); break;
		case DTYPE_INT32: _pTable->changeKey(*(int32*)pszBuf, *(int32*)_pRowData->getData(info), info->_type); break;
		case DTYPE_INT64: _pTable->changeKey(*(int64*)pszBuf, *(int64*)_pRowData->getData(info), info->_type); break;
		case DTYPE_STRING: _pTable->changeKey((const char*)pszBuf, (const char*)_pRowData->getData(info), info->_type); break;
		default: SLASSERT(false, "invaild key type"); break;
		}
	}
	_pRowData->setData(info, pszBuf, size);
}