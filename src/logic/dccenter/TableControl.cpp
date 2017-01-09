#include "TableControl.h"
#include "TableRow.h"

TableControl::TableControl(TableColumn* pTableColumn, IObject* pHost)
	:m_pTableColumn(pTableColumn), m_pHost(pHost)
{}

TableControl::~TableControl(){

}

const IRow* TableControl::getRow(const int32 index) const{
	if (index < 0 || index >= rowCount()){
		SLASSERT(false, "out of range");
		return nullptr;
	}
	return m_tableRows[index];
}

const IRow* TableControl::findRow(const int64 key) const {
	if (m_pTableColumn->getKeyType() == DTYPE_INT8
		|| m_pTableColumn->getKeyType() == DTYPE_INT16
		|| m_pTableColumn->getKeyType() == DTYPE_INT32
		|| m_pTableColumn->getKeyType() == DTYPE_INT64){
		auto itor = m_keyToColIdx.find(key);
		if (itor != m_keyToColIdx.end()){
			return getRow(itor->second);
		}
	}
	return nullptr;
}

const IRow* TableControl::findRow(const char* key) const{
	SLASSERT(m_pTableColumn->getKeyType() == DTYPE_STRING, "wtf");
	if (m_pTableColumn->getKeyType() == DTYPE_STRING){
		auto itor = m_strToColIdx.find(key);
		if (itor != m_strToColIdx.end()){
			return getRow(itor->second);
		}
	}
	return nullptr;
}

void TableControl::clearRows(){
	for (int32 i = 0; i < (int32)m_tableRows.size(); i++){
		DEL m_tableRows[i];
	}
	m_tableRows.clear();
	m_strToColIdx.clear();
	m_keyToColIdx.clear();
}
IRow* TableControl::addRow(){
	SLASSERT(m_pTableColumn->getKeyType() == DTYPE_CANT_BE_KEY, "wtf");
	if (m_pTableColumn->getKeyType() == DTYPE_CANT_BE_KEY){
		TableRow* row = NEW TableRow(this, m_pTableColumn);
		m_tableRows.push_back(row);
		return row;
	}
	return nullptr;
}

IRow* TableControl::addRowKey(const int8 type, const void * data, const int32 size, const int64 key){
	SLASSERT(m_pTableColumn->getKeyType() == type, "wtf");
	if (m_pTableColumn->getKeyType() == type){
		TableRow* row = NEW TableRow(this, m_pTableColumn);
		m_keyToColIdx.insert(make_pair(key, (int32)m_tableRows.size()));
		row->setData(m_pTableColumn->getKeyColumn(), type, data, size, false);
		m_tableRows.push_back(row);
		return row;
	}
	return nullptr;
}

IRow* TableControl::addRowKeyString(const char* key){
	SLASSERT(m_pTableColumn->getKeyType() == DTYPE_STRING, "wtf");
	if (m_pTableColumn->getKeyType() == DTYPE_STRING){
		TableRow* row = NEW TableRow(this, m_pTableColumn);
		m_strToColIdx.insert(make_pair(key, (int32)m_tableRows.size()));
		row->setData(m_pTableColumn->getKeyColumn(), DTYPE_STRING, key, (int32)strlen(key) + 1, false);
		m_tableRows.push_back(row);
		return row;
	}
	return nullptr;
}

bool TableControl::delRow(const int32 index){
	const IRow* row = getRow(index);
	if (!row){
		SLASSERT(false, "have no data on index %d", index);
		return false;
	}

	switch (m_pTableColumn->getKeyType()){
	case DTYPE_INT8:{
		int8 key = row->getDataInt8(m_pTableColumn->getKeyColumn());
		m_keyToColIdx.erase(key);
		break;
	}
	case DTYPE_INT16:{
		int16 key = row->getDataInt16(m_pTableColumn->getKeyColumn());
		m_keyToColIdx.erase(key);
		break;
	}
	case DTYPE_INT32:{
		int32 key = row->getDataInt32(m_pTableColumn->getKeyColumn());
		m_keyToColIdx.erase(key);
		break;
	}
	case DTYPE_INT64:{
		int64 key = row->getDataInt64(m_pTableColumn->getKeyColumn());
		m_keyToColIdx.erase(key);
		break;
	}
	case DTYPE_STRING:{
		const char* key = row->getDataString(m_pTableColumn->getKeyColumn());
		m_strToColIdx.erase(key);
		break;
	}
	case DTYPE_CANT_BE_KEY:
		break;
	default:
		SLASSERT(false, "invaild key type");
		break;
	}

	auto itor = m_tableRows.begin() + index;
	m_tableRows.erase(itor);
	DEL row;
	updateRowKeyIndex(index);
	
	return true;
}

void TableControl::updateRowKeyIndex(const int32 index){
	for (int32 i = index; i < (int32)m_tableRows.size(); i++){
		switch (m_pTableColumn->getKeyType())
		{
		case DTYPE_INT8:{
			int8 key = m_tableRows[i]->getDataInt8(m_pTableColumn->getKeyColumn());
			m_keyToColIdx[key] = i;
			break;
		}
		case DTYPE_INT16:{
			int16 key = m_tableRows[i]->getDataInt16(m_pTableColumn->getKeyColumn());
			m_keyToColIdx[key] = i;
			break;
		}
		case DTYPE_INT32:{
			int32 key = m_tableRows[i]->getDataInt32(m_pTableColumn->getKeyColumn());
			m_keyToColIdx[key] = i;
			break;
		}
		case DTYPE_INT64:{
			int64 key = m_tableRows[i]->getDataInt64(m_pTableColumn->getKeyColumn());
			m_keyToColIdx[key] = i;
			break;
		}
		case DTYPE_STRING:{
			const char* key = m_tableRows[i]->getDataString(m_pTableColumn->getKeyColumn());
			m_strToColIdx[key] = i;
			break;
		}
		case DTYPE_CANT_BE_KEY:
			break;
		default:
			SLASSERT(false, "invaild key type");
			break;
		}
		m_tableRows[i]->setRowIndex(i);
	}
}