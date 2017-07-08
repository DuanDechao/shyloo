#include "MMObject.h"
#include "TableControl.h"
#include "ObjectProp.h"
#include "ObjectFSM.h"

MMObject::MMObject(const char* name, const ObjectPropInfo* pPropInfo)
	:_name(name),
    _objectId(0),
    _isShadow(false),
	_poPropInfo(pPropInfo),
    _memory(nullptr),
	_objectFSM(nullptr)
{
	_memory = NEW OMemory(_poPropInfo->getMemSize());
	_poPropInfo->queryTables([this](const int32 name, const TableColumn* pTableColumn){
		TableControl * pTable = NEW TableControl(name, pTableColumn, this);
		_tables[name] = pTable;
	});
	_objectFSM = NEW ObjectFSM();
}

MMObject::~MMObject(){
	if (_memory)
		DEL _memory;
	_memory = nullptr;

	for (auto table = _tables.begin(); table != _tables.end(); ++table){
		DEL table->second;
	}
	_tables.clear();

	if (_objectFSM)
		DEL _objectFSM;
	_objectFSM = nullptr;
}

const std::vector<const IProp*>& MMObject::getObjProps(bool noParent) const{
	return _poPropInfo->getObjectProp(noParent);
}

bool MMObject::setData(const IProp* prop, const bool temp, const int8 type, const void* data, const int32 size, const bool sync){
	const PropLayout* layout = ((ObjectProp*)prop)->getLayout(_poPropInfo->getObjTypeId());
	//SLASSERT(layout, "wtf");
	if (layout != nullptr){
		//SLASSERT(layout->_type == type && layout->_size >= size && layout->_isTemp == temp, "wtf");
		if (layout->_type == type && layout->_size >= size){
			_memory->setData(layout, data, size);
			if (!layout->_isTemp)
				propCall(prop, sync);
			return true;
		}
	}
	return false;
}

const void* MMObject::getData(const IProp* prop, const bool temp, const int8 type, int32& size)const{
	const PropLayout* layout = ((ObjectProp*)prop)->getLayout(_poPropInfo->getObjTypeId());
	//SLASSERT(layout, "wtf");
	if (layout != nullptr){
		//SLASSERT(layout->_type == type && layout->_size >= size && layout->_isTemp == temp, "wtf");
		if (layout->_type == type && layout->_size >= size){
			size = layout->_size;
			return _memory->getData(layout);
		}
	}
	return nullptr;
}

ITableControl* MMObject::findTable(const int32 name) const{
	auto itor = _tables.find(name);
	if (itor != _tables.end()){
		return itor->second;
	}
	return nullptr;
}
