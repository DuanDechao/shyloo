#include "MMObject.h"
#include "TableControl.h"
#include "ObjectProp.h"
MMObject::MMObject(const char* name, const ObjectPropInfo* pPropInfo)
	:m_name(name),
	m_poPropInfo(pPropInfo),
	m_objectId(0)
{
	m_memory = NEW OMemory(m_poPropInfo->getMemSize());
	m_poPropInfo->queryTables([this](const int32 name, const TableColumn* pTableColumn){
		TableControl * pTable = NEW TableControl(name, pTableColumn, this);
		m_tables[name] = pTable;
	});
}

MMObject::~MMObject(){
	DEL m_memory;
	for (auto& table = m_tables.begin(); table != m_tables.end(); ++table){
		DEL table->second;
	}
	m_tables.clear();
}

const std::vector<const IProp*>& MMObject::getObjProps(bool noParent) const{
	return m_poPropInfo->getObjectProp(noParent);
}

bool MMObject::setData(const IProp* prop, const int8 type, const void* data, const int32 size){
	const PropLayout* layout = ((ObjectProp*)prop)->getLayout(m_poPropInfo->getObjTypeId());
	SLASSERT(layout, "wtf");
	if (layout != nullptr){
		SLASSERT(layout->_type == type && layout->_size >= size, "wtf");
		if (layout->_type == type && layout->_size >= size){
			m_memory->setData(layout, data, size);
			return true;
		}
	}
	return false;
}

const void* MMObject::getData(const IProp* prop, const int8 type, int32& size)const{
	const PropLayout* layout = ((ObjectProp*)prop)->getLayout(m_poPropInfo->getObjTypeId());
	SLASSERT(layout, "wtf");
	if (layout != nullptr){
		SLASSERT(layout->_type == type && layout->_size >= size, "wtf");
		if (layout->_type == type && layout->_size >= size){
			size = layout->_size;
			return m_memory->getData(layout);
		}
	}
	return nullptr;
}

ITabelControl* MMObject::findTable(const int32 name) const{
	auto itor = m_tables.find(name);
	if (itor != m_tables.end()){
		return itor->second;
	}
	return nullptr;
}