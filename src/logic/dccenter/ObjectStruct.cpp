#include "ObjectStruct.h"

bool ObjectPropInfo::loadProp(const sl::ISLXmlNode& props, PROP_DEFDINE_MAP define){
	for (int32 i = 0; i < props.count(); i++){
		PropLayout layout;
		layout._name = props[i].getAttributeString("name");
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

		layout._offset = m_size;
		m_size += layout._size;
		
		layout._setting = 0;
		for (auto& def : define){
			if (props[i].hasAttribute(def.first.c_str()) && props[i].getAttributeBoolean(def.first.c_str())){
				layout._setting |= def.second;
			}
		}
		m_layouts.push_back(layout);
	}
}