#ifndef __OBJECT_STRUCT_H_
#define __OBJECT_STRUCT_H_
#include "sltools.h"
#include "IDCCenter.h"
#include "TableRow.h"
#include "slxml_reader.h"
#include <unordered_map>

struct PropLayout : public MemLayout{
	int8	_type;
	int32	_setting;
	sl::SLString<MAX_PROP_NAME_LEN> _name;
};

typedef std::unordered_map<sl::SLString<MAX_PROP_NAME_LEN>, int32, sl::HashFunc<MAX_PROP_NAME_LEN>, sl::EqualFunc<MAX_PROP_NAME_LEN>> PROP_DEFDINE_MAP;

class ObjectPropInfo{
public:
	ObjectPropInfo(int32 objTypeId, const char* objName, ObjectPropInfo* parenter);

private:
	bool loadProp(const sl::ISLXmlNode& prop, PROP_DEFDINE_MAP define);
	bool loadTable(const sl::ISLXmlNode& table);
private:
	int32				m_objTypeId;
	vector<PropLayout>	m_layouts;
	vector<IProp*>		m_props;
	vector<IProp*>		m_selfProps;
	sl::SLString<MAX_OBJECT_NAME_LEN> m_objName;
	int32				m_size;
};

//class ObjectPropInfo{
//public:
//	typedef std::unordered_map<int32, PropInfo> PROP_MAP;
//	typedef std::vector<PropInfo> PROP_INDEX;
//
//	ObjectPropInfo()
//		:m_propIndex(),
//		m_propMap(),
//		m_index(0),
//		m_memorySize(0)
//	{}
//
//	ObjectPropInfo(const ObjectPropInfo& propInfo)
//		:m_propIndex(propInfo.m_propIndex),
//		m_propMap(propInfo.m_propMap),
//		m_index(propInfo.m_index),
//		m_memorySize(propInfo.m_memorySize)
//	{}
//
//	PropInfo* addProp(const int32 name, const char* realname, int8 mask, int32 size, int32 setting){
//
//		PropInfo prop(name, realname, m_index, m_memorySize, size, mask, setting);
//		m_propIndex.push_back(prop);
//		m_propMap.insert(make_pair(name, prop));
//		m_memorySize += size;
//		m_index++;
//		return &m_propIndex[m_propIndex.size() - 1];
//	}
//
//	PropInfo* queryProp(const int32 prop){
//		if (m_propMap.find(prop) == m_propMap.end()){
//			SLASSERT(false, "cannnot find prop %d", prop);
//			return nullptr;
//		}
//		return &m_propMap[prop];
//	}
//
//	int32 getMemorySize() const {return m_memorySize;}
//
//private:
//	PROP_INDEX	m_propIndex;
//	PROP_MAP	m_propMap;
//	int32		m_index;
//	int32		m_memorySize;
//};
//
//class ObjectProp{
//public:
//	ObjectProp(ObjectPropInfo* propInfo) :m_pObjectProp(propInfo),m_memorySize(propInfo->getMemorySize()),m_propData(nullptr){
//		m_propData = NEW char[m_memorySize];
//		sl::SafeMemset(m_propData, sizeof(m_propData), 0, sizeof(int32));
//	}
//	~ObjectProp(){
//		DEL[] m_propData;
//		m_pObjectProp = nullptr;
//		m_propData = nullptr;
//		m_memorySize = 0;
//	}
//
//	bool setValue(const int32 prop, const int8 mask, const char* value, int32 size){
//		SLASSERT(m_pObjectProp, "wtf");
//		PropInfo* info = m_pObjectProp->queryProp(prop);
//		if (!info || info->_mask != mask || info->_name != prop || info->_size < size){
//			SLASSERT(false, "invaild prop %d", prop);
//			return false;
//		}
//		sl::SafeMemcpy(m_propData + info->_offset, info->_size, value, size);
//		return true;
//	}
//
//	const char* getValue(const int32 prop, const int8 mask){
//		SLASSERT(m_pObjectProp && m_propData, "wtf");
//		PropInfo* info = m_pObjectProp->queryProp(prop);
//		if (!info || info->_mask != mask || info->_name != prop || (info->_offset + info->_size) > m_memorySize){
//			SLASSERT(false, "invaild prop %d", prop);
//			return nullptr;
//		}
//		return &m_propData[info->_offset];
//	}
//
//	void clear(){
//		if (!m_propData){
//			SLASSERT(false, "invaild data");
//			return;
//		}
//		sl::SafeMemset(m_propData, sizeof(m_propData), 0, sizeof(int32));
//	}
//private:
//	ObjectPropInfo* m_pObjectProp;
//	char*	m_propData;
//	int32	m_memorySize;
//};
#endif