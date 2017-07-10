#include "TableControl.h"
#include "TableRow.h"

TableControl::TableControl(const int32 name, const TableColumn* pTableColumn, IObject* pHost)
	:_name(name), _pTableColumn(pTableColumn), _pHost(pHost)
{}

TableControl::~TableControl(){
	for (auto* row : _tableRows){
		if (row)
			DEL row;
	}
	_tableRows.clear();
	_strToColIdx.clear();
	_keyToColIdx.clear();
}

const IRow* TableControl::getRow(const int32 index) const{
	if (index < 0 || index >= rowCount()){
		SLASSERT(false, "out of range");
		return nullptr;
	}
	return _tableRows[index];
}

const IRow* TableControl::findRow(const int64 key) const {
	if (_pTableColumn->getKeyType() == DTYPE_INT8
		|| _pTableColumn->getKeyType() == DTYPE_INT16
		|| _pTableColumn->getKeyType() == DTYPE_INT32
		|| _pTableColumn->getKeyType() == DTYPE_INT64){
		auto itor = _keyToColIdx.find(key);
		if (itor != _keyToColIdx.end()){
			return getRow(itor->second);
		}
	}
	return nullptr;
}

const IRow* TableControl::findRow(const char* key) const{
	SLASSERT(_pTableColumn->getKeyType() == DTYPE_STRING, "wtf");
	if (_pTableColumn->getKeyType() == DTYPE_STRING){
		auto itor = _strToColIdx.find(key);
		if (itor != _strToColIdx.end()){
			return getRow(itor->second);
		}
	}
	return nullptr;
}

void TableControl::clearRows(){
	for (auto* row : _tableRows){
		if (row)
			DEL row;
	}
	_tableRows.clear();
	_strToColIdx.clear();
	_keyToColIdx.clear();
}
IRow* TableControl::addRow(){
	SLASSERT(_pTableColumn->getKeyType() == DTYPE_CANT_BE_KEY, "wtf");
	if (_pTableColumn->getKeyType() == DTYPE_CANT_BE_KEY){
		TableRow* row = NEW TableRow(this, _pTableColumn);
		row->setRowIndex((int32)_tableRows.size());
		_tableRows.push_back(row);
		return row;
	}
	return nullptr;
}

IRow* TableControl::addRowKey(const int8 type, const void * data, const int32 size, const int64 key){
	SLASSERT(_pTableColumn->getKeyType() == type, "wtf");
	if (_pTableColumn->getKeyType() == type){
		TableRow* row = NEW TableRow(this, _pTableColumn);
		_keyToColIdx.insert(make_pair(key, (int32)_tableRows.size()));
		row->setData(_pTableColumn->getKeyColumn(), type, data, size, false);
		row->setRowIndex((int32)_tableRows.size());
		_tableRows.push_back(row);
		return row;
	}
	return nullptr;
}

IRow* TableControl::addRowKeyString(const char* key){
	SLASSERT(_pTableColumn->getKeyType() == DTYPE_STRING, "wtf");
	if (_pTableColumn->getKeyType() == DTYPE_STRING){
		TableRow* row = NEW TableRow(this, _pTableColumn);
		_strToColIdx.insert(make_pair(key, (int32)_tableRows.size()));
		row->setData(_pTableColumn->getKeyColumn(), DTYPE_STRING, key, (int32)strlen(key) + 1, false);
		row->setRowIndex((int32)_tableRows.size());
		_tableRows.push_back(row);
		return row;
	}
	return nullptr;
}

bool TableControl::swapRowIndex(const int32 src, const int32 dst){
	if (src == dst)
		return true;

	TableRow* srcRow = (TableRow*)getRow(src);
	if (!srcRow){
		return false;
	}

	TableRow* dstRow = (TableRow*)getRow(dst);
	if (!dstRow){
		return false;
	}

	const int8 keyType = _pTableColumn->getKeyType();
	switch (keyType){
	case DTYPE_INT8:{
		int8 srcKey = srcRow->getDataInt8(_pTableColumn->getKeyColumn());
		int8 dstKey = dstRow->getDataInt8(_pTableColumn->getKeyColumn());
		_keyToColIdx[srcKey] = dst;
		_keyToColIdx[dstKey] = src;
		break;
	}
	case DTYPE_INT16:{
		int16 srcKey = srcRow->getDataInt16(_pTableColumn->getKeyColumn());
		int16 dstKey = dstRow->getDataInt16(_pTableColumn->getKeyColumn());
		_keyToColIdx[srcKey] = dst;
		_keyToColIdx[dstKey] = src;
		break;
	}
	case DTYPE_INT32:{
		int32 srcKey = srcRow->getDataInt32(_pTableColumn->getKeyColumn());
		int32 dstKey = dstRow->getDataInt32(_pTableColumn->getKeyColumn());
		_keyToColIdx[srcKey] = dst;
		_keyToColIdx[dstKey] = src;
		break;
	}
	case DTYPE_INT64:{
		int64 srcKey = srcRow->getDataInt64(_pTableColumn->getKeyColumn());
		int64 dstKey = dstRow->getDataInt64(_pTableColumn->getKeyColumn());
		_keyToColIdx[srcKey] = dst;
		_keyToColIdx[dstKey] = src;
		break;
	}
	case DTYPE_STRING:{
		const char* srcKey = srcRow->getDataString(_pTableColumn->getKeyColumn());
		const char* dstKey = dstRow->getDataString(_pTableColumn->getKeyColumn());
		_strToColIdx[srcKey] = dst;
		_strToColIdx[dstKey] = src;
		break;
	}
	default:
		break;
	}

	_tableRows[src] = dstRow;
	_tableRows[dst] = srcRow;

	srcRow->setRowIndex(dst);
	dstRow->setRowIndex(src);

	return true;
}

bool TableControl::delRow(const int32 index){
	const IRow* row = getRow(index);
	if (!row){
		SLASSERT(false, "have no data on index %d", index);
		return false;
	}

	switch (_pTableColumn->getKeyType()){
	case DTYPE_INT8:{
		int8 key = row->getDataInt8(_pTableColumn->getKeyColumn());
		_keyToColIdx.erase(key);
		break;
	}
	case DTYPE_INT16:{
		int16 key = row->getDataInt16(_pTableColumn->getKeyColumn());
		_keyToColIdx.erase(key);
		break;
	}
	case DTYPE_INT32:{
		int32 key = row->getDataInt32(_pTableColumn->getKeyColumn());
		_keyToColIdx.erase(key);
		break;
	}
	case DTYPE_INT64:{
		int64 key = row->getDataInt64(_pTableColumn->getKeyColumn());
		_keyToColIdx.erase(key);
		break;
	}
	case DTYPE_STRING:{
		const char* key = row->getDataString(_pTableColumn->getKeyColumn());
		_strToColIdx.erase(key);
		break;
	}
	case DTYPE_CANT_BE_KEY:
		break;
	default:
		SLASSERT(false, "invaild key type");
		break;
	}

	auto itor = _tableRows.begin() + index;
	_tableRows.erase(itor);
	DEL row;
	updateRowKeyIndex(index);
	
	return true;
}

void TableControl::changeKey(const int64 newKey, const int64 oldKey, const int8 type){
	SLASSERT(_pTableColumn->getKeyType() == type, "wtf");
	if (newKey == oldKey)
		return;
	
	_keyToColIdx[newKey] = _keyToColIdx[oldKey];
	_keyToColIdx.erase(oldKey);
}

void TableControl::changeKey(const char* newKey, const char* oldKey, const int8 type){
	SLASSERT(_pTableColumn->getKeyType() == type, "wtf");
	if (newKey == oldKey)
		return;

	_strToColIdx[newKey] = _strToColIdx[oldKey];
	_strToColIdx.erase(oldKey);
}

void TableControl::updateRowKeyIndex(const int32 index){
	for (int32 i = index; i < (int32)_tableRows.size(); i++){
		switch (_pTableColumn->getKeyType())
		{
		case DTYPE_INT8:{
			int8 key = _tableRows[i]->getDataInt8(_pTableColumn->getKeyColumn());
			_keyToColIdx[key] = i;
			break;
		}
		case DTYPE_INT16:{
			int16 key = _tableRows[i]->getDataInt16(_pTableColumn->getKeyColumn());
			_keyToColIdx[key] = i;
			break;
		}
		case DTYPE_INT32:{
			int32 key = _tableRows[i]->getDataInt32(_pTableColumn->getKeyColumn());
			_keyToColIdx[key] = i;
			break;
		}
		case DTYPE_INT64:{
			int64 key = _tableRows[i]->getDataInt64(_pTableColumn->getKeyColumn());
			_keyToColIdx[key] = i;
			break;
		}
		case DTYPE_STRING:{
			const char* key = _tableRows[i]->getDataString(_pTableColumn->getKeyColumn());
			_strToColIdx[key] = i;
			break;
		}
		case DTYPE_CANT_BE_KEY:
			break;
		default:
			SLASSERT(false, "invaild key type");
			break;
		}
		_tableRows[i]->setRowIndex(i);
	}
}