#ifndef SL_LOGIC_SCRIPT_DEF_MODULE_H
#define SL_LOGIC_SCRIPT_DEF_MODULE_H
#include "slikernel.h"
#include "slsingleton.h"
#include <unordered_map>
#include "slstring.h"
#include "GameDefine.h"
#include <vector>
#include "slxml_reader.h"
#include "IEntityDef.h"
#define MAX_PROP_NAME_LEN 64
#define MAX_OBJECT_NAME_LEN 64

struct PropLayout{
	int8	_type;
	int32	_flags;
	int32   _index;
	int32   _size;
	sl::SLString<MAX_PROP_NAME_LEN> _name;
};

class IProp;
class IObject;
class ScriptDefModule: public IScriptDefModule{
public:
	//entity �����ݴ������Ա��
	enum EntityDataFlags{
		ED_FLAG_UNKNOWN = 0x00000000,						//δ����
		ED_FLAG_CELL_PUBLIC = 0x00000001,					//�������cell�㲥
		ED_FLAG_CELL_PRIVATE = 0x00000002,					//��ǰcell
		ED_FLAG_ALL_CLIENTS = 0x00000004,					//cell�㲥�����пͻ���
		ED_FLAG_CELL_PUBLIC_AND_OWN = 0x00000008,			//cell�㲥���Լ��Ŀͻ���
		ED_FLAG_OWN_CLIENT = 0x00000010,					//��ǰcell�Ϳͻ���
		ED_FLAG_BASE_AND_CLIENT = 0x00000020,				//base�Ϳͻ���
		ED_FLAG_BASE = 0x00000040,							//��ǰbase
		ED_FLAG_OTHER_CLIENTS = 0x00000080,					//cell�㲥�������ͻ���
		ED_FLAG_MASK = 0x0000FFFF,							//flags����
	};

	//entity�������Ա��
	enum EntityDataFlags1{
		ED_FLAG1_UNKNOWN = 0x00000000,						//δ����
		ED_FLAG1_PERSISTENT = 0x00010000,					//�Ƿ�洢���ݿ�
		ED_FLAG1_IDENTIFIER = 0x00020000,					//�Ƿ��Ǳ�ʶ
		ED_FLAG1_MASK = 0xFFFF0000,							//flags����
	};

	//
	enum EntityDataFlagRelation{
		//������baseapp�й�ϵ�ı�־
		ENTITY_BASE_DATA_FLAGS = ED_FLAG_BASE | ED_FLAG_BASE_AND_CLIENT,
		//������cellapp��صı�־
		ENTITY_CELL_DATA_FLAGS = ED_FLAG_CELL_PUBLIC | ED_FLAG_CELL_PRIVATE | ED_FLAG_ALL_CLIENTS | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OTHER_CLIENTS | ED_FLAG_OWN_CLIENT,
		//������client�йصı�־
		ENTITY_CLIENT_DATA_FLAGS = ED_FLAG_BASE_AND_CLIENT | ED_FLAG_ALL_CLIENTS | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OTHER_CLIENTS | ED_FLAG_OWN_CLIENT,
		//
	};

	ScriptDefModule(const char* moduleName, ScriptDefModule* parentModule);
	~ScriptDefModule();

	static bool initialize();

	bool loadParentModule(ScriptDefModule* parentModule);
	bool loadFrom(const sl::ISLXmlNode& root);

	inline bool hasBase() const { return _hasBase; }
	inline bool hasCell() const { return _hasCell; }
	inline bool hasClient() const { return _hasClient; }
	inline const std::vector<PropLayout*>& propLayouts() { return _layouts; }

	inline void setScriptType(PyTypeObject* pyType) { _scriptType = pyType; }

	virtual PyObject* scriptGetObjectAttribute(PyObject* object, PyObject* attr);
	virtual int32 scriptSetObjectAttribute(PyObject* object, PyObject* attr, PyObject* value);
	virtual PyObject* createObject(void);
	virtual const char* getModuleName() const { return _moduleName.c_str(); }
	virtual IObject* getMMObject(PyObject* object);

private:
	bool loadAllDefDescriptions(const char* moduleName, const sl::ISLXmlNode& root);
	bool loadDefPropertys(const char* moduleName, const sl::ISLXmlNode& root);
	bool loadDefCellMethods(const char* moduleName, const sl::ISLXmlNode& root);
	bool loadDefBaseMethods(const char* moduleName, const sl::ISLXmlNode& root);
	bool loadDefClientMethods(const char* moduleName, const sl::ISLXmlNode& root);
	bool loadDefMethods(const char* moduleName, const int8 type, const sl::ISLXmlNode& root);

	const IProp* getProp(PyObject* attr);
	bool appendObjectProp(PropLayout* layout);
	PyObject* getAttributeValue(IObject* object, const IProp* prop);

private:
	typedef std::unordered_map<std::string, EntityDataFlags> ENTITY_FLAGS_MAP;
	//�ű����
	PyTypeObject*						_scriptType;
	sl::SLString<MAX_OBJECT_NAME_LEN>	_moduleName;
	static ENTITY_FLAGS_MAP				_entityFlagMapping;
	bool								_hasBase;
	bool								_hasCell;
	bool								_hasClient;
	std::vector<PropLayout*>			_layouts;

	//�����ֵ�
	PyObject*							_propDict;
};
#endif