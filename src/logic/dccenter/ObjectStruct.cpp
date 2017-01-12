#include "ObjectStruct.h"
#include "sltools.h"
ObjectPropInfo::ObjectPropInfo(int32 objTypeId, const char* objName, ObjectPropInfo* parenter)
	:m_objTypeId(objTypeId), m_objName(objName), m_size(0){
	if (parenter){
		m_layouts = parenter->m_layouts;
		for (auto& layout £º m_layouts){
			const IProp* prop = ObjectMgr::setObjectProp(layout._name.c_str(), m_objTypeId, &layout);
			m_props.push_back(prop);
		}
		m_size = parenter->m_size;
	}
}

bool ObjectPropInfo::loadFrom(const sl::ISLXmlNode& root, PROP_DEFDINE_MAP& defines){
	if (!loadProps(root["prop"], defines))
		return false;

	if (root.subNodeExist("table")){
		return loadTables(root["table"]);
	}
	return true;
}

bool ObjectPropInfo::loadProps(const sl::ISLXmlNode& props, PROP_DEFDINE_MAP& defines){
	for (int32 i = 0; i < props.count(); i++){
		PropLayout layout;
		layout._name = props[i].getAttributeString("name");
		layout._offset = m_size;
		const char* type = props[i].getAttributeString("type");
		if (!strcmp(type, "int8")){
			layout._type = DTYPE_INT8;
			layout._size = sizeof(int8);
		}
		else if (!strcmp(type, "int16")){
			layout._type = DTYPE_INT16;
			layout._size = sizeof(int16);
		}
		else if (!strcmp(type, "int32")){
			layout._type = DTYPE_INT32;
			layout._size = sizeof(int32);
		}
		else if (!strcmp(type, "int64")){
			layout._type = DTYPE_INT64;
			layout._size = sizeof(int64);
		}
		else if (!strcmp(type, "float")){
			layout._type = DTYPE_FLOAT;
			layout._size = sizeof(float);
		}
		else{
			SLASSERT(false, "invaild prop type %s", type);
			return false;
		}

		m_size += layout._size;
		layout._setting = 0;
		for (auto& def : define){
			if (props[i].hasAttribute(def.first.c_str()) && props[i].getAttributeBoolean(def.first.c_str())){
				layout._setting |= def.second;
			}
		}

		m_layouts.push_back(layout);

		const IProp * prop = ObjectMgr::setObjectProp(layout._name.c_str(), m_objTypeId, &(*m_layouts.rbegin()));
		m_props.push_back(prop);
		m_selfProps.push_back(prop);
	}
	return true;
}

bool ObjectPropInfo::loadTables(const sl::ISLXmlNode& tables){
	for (int32 i = 0; i < tables.count(); i++){
		const char* name = tables[i].getAttributeString("name");
		TableColumn* pNewTable = NEW TableColumn();
		if (!pNewTable->loadColumnConfig(tables[i])){
			return false;
		}
		TableInfo tableInfo{ sl::CalcStringUniqueId(name), pNewTable };
		m_tables.push_back(tableInfo);
	}
	return true;
}