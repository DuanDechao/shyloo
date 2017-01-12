#ifndef __OBJECT_STRUCT_H_
#define __OBJECT_STRUCT_H_
#include "sltools.h"
#include "IDCCenter.h"
#include "TableRow.h"
#include "slxml_reader.h"
#include <unordered_map>
#include <functional>
struct PropLayout : public MemLayout{
	int8	_type;
	int32	_setting;
	sl::SLString<MAX_PROP_NAME_LEN> _name;
};

typedef std::unordered_map<sl::SLString<MAX_PROP_NAME_LEN>, int32, sl::HashFunc<MAX_PROP_NAME_LEN>, sl::EqualFunc<MAX_PROP_NAME_LEN>> PROP_DEFDINE_MAP;

class ObjectPropInfo{
	struct TableInfo{
		int32 _name;
		TableColumn* _tableColumn;
	};
public:
	ObjectPropInfo(int32 objTypeId, const char* objName, ObjectPropInfo* parenter);
	~ObjectPropInfo();

	inline const int32 getMemSize() const { return m_size; }
	inline const int32 getObjTypeId() const { return m_objTypeId; }
	inline const char* getObjTypeName() const { return m_objName.c_str(); }
	inline vector<const IProp*>& getObjectProp(bool parenter = false){ return parenter ? m_props : m_selfProps; }

	bool loadFrom(const sl::ISLXmlNode& root, PROP_DEFDINE_MAP& defines);

	inline void queryTables(const std::function<void(const int32 name, const TableColumn* pTableColumn)> &f){
		for (auto& table : m_tables){
			f(table._name, table._tableColumn);
		}
	}

private:
	bool loadProps(const sl::ISLXmlNode& prop, PROP_DEFDINE_MAP& defines);
	bool loadTables(const sl::ISLXmlNode& table);

private:
	int32								m_objTypeId;
	vector<PropLayout>					m_layouts;
	vector<const IProp*>				m_props;
	vector<const IProp*>				m_selfProps;
	sl::SLString<MAX_OBJECT_NAME_LEN>	m_objName;
	int32								m_size;
	vector<TableInfo>					m_tables;
};

#endif