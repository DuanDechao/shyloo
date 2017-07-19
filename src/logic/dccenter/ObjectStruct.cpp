#include "ObjectStruct.h"
#include "ObjectMgr.h"
#include "sltools.h"
ObjectPropInfo::ObjectPropInfo(int32 objTypeId, const char* objName, ObjectPropInfo* parenter)
	:_objTypeId(objTypeId), _objName(objName), _size(0){
	if (parenter){
		_layouts = parenter->_layouts;
		for (auto& layout : _layouts){
			if (!layout->_isTemp){
				const IProp* prop = ObjectMgr::getInstance()->setObjectProp(layout->_name.c_str(), _objTypeId, layout);
				_props.push_back(prop);
			}
			else{
				ObjectMgr::getInstance()->setObjectTempProp(layout->_name.c_str(), _objTypeId, layout);
			}
		}

		_tables = parenter->_tables;
		_size = parenter->_size;
	}
}

ObjectPropInfo::~ObjectPropInfo(){
	_objTypeId = 0;
	_objName = "";
	_size = 0;

	/*for (auto tableInfo : _tables){
		if (tableInfo._tableColumn)
		DEL tableInfo._tableColumn;
		}*/

	/*for (auto* layout : _layouts){
		if (layout)
		DEL layout;
		}*/

	_layouts.clear();
	_props.clear();
	_selfProps.clear();
	_tables.clear();
}

bool ObjectPropInfo::loadFrom(const sl::ISLXmlNode& root, PROP_DEFDINE_MAP& defines){
	int32 startIndex = 0;
	if (root.hasAttribute("start"))
		startIndex = root.getAttributeInt32("start");

	if (!loadProps(root["prop"], defines, startIndex))
		return false;

	if (root.subNodeExist("temp") && !loadTemps(root["temp"])){
		return false;
	}

	if (root.subNodeExist("table") && !loadTables(root["table"])){
		return false;
	}
	return true;
}

bool ObjectPropInfo::loadProps(const sl::ISLXmlNode& props, PROP_DEFDINE_MAP& defines, int32 startIndex){
	for (int32 i = 0; i < props.count(); i++){
		PropLayout* layout = NEW PropLayout();
		if (!loadPropConfig(props[i], *layout)){
			return false;
		}

		_size += layout->_size;
		layout->_setting = 0;
		layout->_isTemp = false;
		layout->_index = startIndex++;

		for (auto& def : defines){
			if (props[i].hasAttribute(def.first.c_str()) && props[i].getAttributeBoolean(def.first.c_str())){
				layout->_setting |= def.second;
			}
		}

		_layouts.push_back(layout);

		const IProp * prop = ObjectMgr::getInstance()->setObjectProp(layout->_name.c_str(), _objTypeId, (*_layouts.rbegin()));
		_props.push_back(prop);
		_selfProps.push_back(prop);
	}
	return true;
}

bool ObjectPropInfo::loadTemps(const sl::ISLXmlNode& temps){
	for (int32 i = 0; i < temps.count(); i++){
		PropLayout* layout = NEW PropLayout();
		if (!loadPropConfig(temps[i], *layout)){
			return false;
		}
		_size += layout->_size;
		layout->_setting = 0;
		layout->_isTemp = true;
		_layouts.push_back(layout);

		ObjectMgr::getInstance()->setObjectTempProp(layout->_name.c_str(), _objTypeId, (*_layouts.rbegin()));
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
		_tables.push_back(tableInfo);
	}
	return true;
}

bool ObjectPropInfo::loadPropConfig(const sl::ISLXmlNode& prop, PropLayout& layout){
	layout._name = prop.getAttributeString("name");
	layout._offset = _size;
	const char* type = prop.getAttributeString("type");
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
	else if (!strcmp(type, "string")){
		int32 size = prop.getAttributeInt32("size");
		layout._type = DTYPE_STRING;
		layout._size = size;
	}
	else{
		SLASSERT(false, "invaild prop type %s", type);
		return false;
	}
	return true;
}