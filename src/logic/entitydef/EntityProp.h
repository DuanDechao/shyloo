#ifndef SL_LOGIC_ENTITY_PROP_H
#define SL_LOGIC_ENTITY_PROP_H
#include "slikernel.h"
#include "slsingleton.h"
#include <unordered_map>
#include "slstring.h"
#include "GameDefine.h"
#include <vector>
#include "slxml_reader.h"

#define MAX_PROP_NAME_LEN 64
#define MAX_OBJECT_NAME_LEN 64
struct PropLayout{
	int8	_type;
	int32	_setting;
	int32   _index;
	int32   _size;
	sl::SLString<MAX_PROP_NAME_LEN> _name;
};

typedef std::unordered_map<sl::SLString<MAX_PROP_NAME_LEN>, int32> PROP_DEFDINE_MAP;
class EntityProp{
public:
	EntityProp(const char* objName, EntityProp* parenter);
	~EntityProp();

	bool loadFrom(const sl::ISLXmlNode& root, PROP_DEFDINE_MAP& defines);

private:
	bool loadProps(const sl::ISLXmlNode& prop, PROP_DEFDINE_MAP& defines, int32 startIndex);
	bool loadPropConfig(const sl::ISLXmlNode& prop, PropLayout& layout);

private:
	std::vector<PropLayout*>			_layouts;
	sl::SLString<MAX_OBJECT_NAME_LEN>	_objName;
};
#endif