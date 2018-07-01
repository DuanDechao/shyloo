#include "GlobalData.h"
#include "IDCCenter.h"
GlobalData::GlobalData(){
	initGlobalDataTable();
}

GlobalData::~GlobalData(){
}

void GlobalData::initGlobalDataTable(){
	SLMODULE(ObjectMgr)->appendTableColumnInfo(GLOBAL_DATA::TABLE_NAME, DTYPE_STRING, 128, true);
	SLMODULE(ObjectMgr)->appendTableColumnInfo(GLOBAL_DATA::TABLE_NAME, DTYPE_INT16, sizeof(int16), false);
	SLMODULE(ObjectMgr)->appendTableColumnInfo(GLOBAL_DATA::TABLE_NAME, DTYPE_INT32, sizeof(int32), false);
	SLMODULE(ObjectMgr)->appendTableColumnInfo(GLOBAL_DATA::TABLE_NAME, DTYPE_BLOB, 1024, false);
	
	_data = CREATE_STATIC_TABLE(SLMODULE(ObjectMgr), GLOBAL_DATA::TABLE_NAME, GLOBAL_DATA::TABLE_NAME);
}

bool GlobalData::write(const char* key, const int16 dataType, const void* data, const int32 size){
	if(!key || !data || size <= 0)
		return false;

	const IRow* row = _data->findRow(key);
	if(!row){
		row = _data->addRowKeyString(key);
		_keys.insert(key);
	}

	row->setDataInt16(GLOBAL_DATA::COLUMN::TYPE, dataType);
	row->setDataInt32(GLOBAL_DATA::COLUMN::DATA_SIZE, size);
	row->setDataBlob(GLOBAL_DATA::COLUMN::DATA, data, size);
	
	onDataChanged(key, dataType, data, size);
	return true;	
}

bool GlobalData::del(const char* key){
	if(!key)
		return false;

	const IRow* row = _data->findRow(key);
	if(!row)
		return false;

	const int16 dataType = row->getDataInt16(GLOBAL_DATA::COLUMN::TYPE);
	const int32 dataSize = row->getDataInt32(GLOBAL_DATA::COLUMN::DATA_SIZE);
	int32 size = 0;
	const void* data = row->getDataBlob(GLOBAL_DATA::COLUMN::DATA, size);

	onDataChanged(key, dataType, data, dataSize, true);
	_data->delRow(row->getRowIndex());

	_keys.erase(_keys.find(key));
	return true;
}

void GlobalData::addListener(IGlobalDataListener* listener){
	_listeners.push_back(listener);
} 

void GlobalData::onDataChanged(const char* key, const int16 dataType, const void* data, const int32 dataSize, bool isDelete){
	for(auto listener : _listeners)
		listener->onGlobalDataChanged(key, dataType, data, dataSize, isDelete);
}

const void* GlobalData::getData(const char* key, int16& dataType, int32& dataSize){
	const IRow* row = _data->findRow(key);
	if(!row){
		ECHO_ERROR("GlobalData::getDataType: GlobalData key[%s] not exist", key);
		return nullptr;
	}
	
	dataType = row->getDataInt16(GLOBAL_DATA::COLUMN::TYPE);
	dataSize = row->getDataInt32(GLOBAL_DATA::COLUMN::DATA_SIZE);
	int32 size = 0;
	return row->getDataBlob(GLOBAL_DATA::COLUMN::DATA, size);
}

bool GlobalData::hasData(const char* key){
	return _data->findRow(key) != NULL;
}

