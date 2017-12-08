#include "EntityProp.h"
#include "slxml_reader.h"
#include "slfile_utils.h"
#include "IDCCenter.h"
EntityProp::EntityProp(const char* objName, EntityProp* parenter)
	:_objName(objName)
{
	if (parenter){
		_layouts = parenter->_layouts;
		for (auto& layout : _layouts){
			SLMODULE(ObjectMgr)->appendObjectProp(objName, layout->_name.c_str(), layout->_type, layout->_size, layout->_setting, layout->_index);
		}
	}
}

EntityProp::~EntityProp(){

}

bool EntityProp::loadFrom(const sl::ISLXmlNode& root, PROP_DEFDINE_MAP& defines){
	int32 startIndex = 0;
	if (root.hasAttribute("start"))
		startIndex = root.getAttributeInt32("start");

	if (!loadProps(root["prop"], defines, startIndex))
		return false;

	return true;
}

bool EntityProp::loadProps(const sl::ISLXmlNode& props, PROP_DEFDINE_MAP& defines, int32 startIndex){
	for (int32 i = 0; i < props.count(); i++){
		PropLayout* layout = NEW PropLayout();
		if (!loadPropConfig(props[i], *layout)){
			return false;
		}

		layout->_setting = 0;
		layout->_index = startIndex++;

		for (auto& def : defines){
			if (props[i].hasAttribute(def.first.c_str()) && props[i].getAttributeBoolean(def.first.c_str())){
				layout->_setting |= def.second;
			}
		}

		SLMODULE(ObjectMgr)->appendObjectProp(_objName.c_str(), layout->_name.c_str(), layout->_type, layout->_size, layout->_setting, layout->_index);

		_layouts.push_back(layout);
	}
	return true;
}

bool EntityProp::loadPropConfig(const sl::ISLXmlNode& prop, PropLayout& layout){
	layout._name = prop.getAttributeString("name");
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

