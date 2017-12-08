#ifndef __OBJECT_STRUCT_H_
#define __OBJECT_STRUCT_H_
#include "sltools.h"
#include "IDCCenter.h"
#include "TableRow.h"
#include "slxml_reader.h"
#include <unordered_map>
#include <functional>
#include "slobject_pool.h"
struct PropLayout : public MemLayout{
	bool    _isTemp;
	int8	_type;
	int32	_setting;
	int32   _index;
	sl::SLString<MAX_PROP_NAME_LEN> _name;
};

#define INVAILD_ARG_TYPE  0xFF
struct MethodLayout{
	MethodLayout() :_argsType(0xFFFFFFFF){}
	int8	_type;
	int32	_setting;
	int32   _index;
	sl::SLString<MAX_METHOD_NAME_LEN> _name;
	uint64	_argsType;

	inline void setArgsType(std::vector<uint8>& argsVec){
		uint8* args = (uint8*)(_argsType);
		for (int32 i = 0; i < argsVec.size(); i++){
			args[i] = argsVec[i];
		}
	}

	inline std::vector<uint8> getArgsType(){
		std::vector<uint8> argsType;
		uint8* args = (uint8*)_argsType;
		for (int32 i = 0; i < sizeof(_argsType); i++){
			if ((uint8)args[i] != INVAILD_ARG_TYPE)
				argsType.push_back((uint8)args[i]);
			else
				break;
		}
		return argsType;
	}
};

typedef std::unordered_map<sl::SLString<MAX_PROP_NAME_LEN>, int32> PROP_DEFDINE_MAP;
class MMObject;
class ObjectPropInfo{
	struct TableInfo{
		int32 _name;
		TableColumn* _tableColumn;
	};
public:
	ObjectPropInfo(int32 objTypeId, const char* objName, ObjectPropInfo* parenter);
	~ObjectPropInfo();

	inline const int32 getMemSize() const { return _size; }
	inline const int32 getObjTypeId() const { return _objTypeId; }
	inline const char* getObjTypeName() const { return _objName.c_str(); }
	inline const vector<const IProp*>& getObjectProp(bool parenter = false) const { return parenter ? _props : _selfProps; }

	bool loadFrom(const sl::ISLXmlNode& root, PROP_DEFDINE_MAP& defines);
	const IProp* loadProp(const char* name, const int8 type, const int32 size, const int32 setting, const int32 index);
	bool loadMethod(const char* name, const int8 type, const int32 setting, const int32 index, vector<uint8>& argsType);

	MMObject* create() const;
	void recover(MMObject* object) const;

	inline void queryTables(const std::function<void(const int32 name, const TableColumn* pTableColumn)> &f) const{
		for (auto& table : _tables){
			f(table._name, table._tableColumn);
		}
	}

private:
	bool loadProps(const sl::ISLXmlNode& prop, PROP_DEFDINE_MAP& defines, int32 startIndex);
	bool loadTemps(const sl::ISLXmlNode& temp);
	bool loadTables(const sl::ISLXmlNode& table);
	bool loadPropConfig(const sl::ISLXmlNode& prop, PropLayout& layout);

private:
	int32								_objTypeId;
	vector<PropLayout*>					_propLayouts;
	vector<MethodLayout*>				_methodLayouts;
	vector<const IProp*>				_props;
	vector<const IProp*>				_selfProps;
	vector<const IMethod*>				_methods;
	vector<const IMethod*>				_selfMethods;
	sl::SLString<MAX_OBJECT_NAME_LEN>	_objName;
	int32								_size;
	vector<TableInfo>					_tables;
	sl::SLOjbectPool<MMObject>*			_objectPool;
};

#endif