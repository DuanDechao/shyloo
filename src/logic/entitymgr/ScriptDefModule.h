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

struct PropDefInfo{
	int8	_type;
	int32	_flags;
	int32   _index;
	int32   _size;
    int64   _extra;
	sl::SLString<MAX_PROP_NAME_LEN> _name;
};

struct MethodDefInfo{
    int8   _type;
    int32  _flags;
    int32  _index;
    vector<uint8> _argsType;
    sl::SLString<MAX_PROP_NAME_LEN> _name; 
};

class IProp;
class IObject;
class IMethod;
class RemoteEntityMethod;
class EntityMailBox;
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

    enum RemoteMethodType{
        RMT_CLIENT = 0,
        RMT_BASE,
        RMT_CELL,
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

	virtual bool hasBase() const { return _hasBase; }
	virtual bool hasCell() const { return _hasCell; }
	virtual bool hasClient() const { return _hasClient; }
	inline const std::vector<PropDefInfo*>& propsDefInfo() { return _propsDefInfo; }
    inline const std::vector<PropDefInfo*>& methodsDefInfo() {return _methodsDefInfo;}

	inline void setScriptType(PyTypeObject* pyType) { _scriptType = pyType; }
   
    virtual PyTypeObject* getScriptType() {return _scriptType;}
	virtual PyObject* scriptGetObjectAttribute(PyObject* object, PyObject* attr);
	virtual int32 scriptSetObjectAttribute(PyObject* object, PyObject* attr, PyObject* value);
    virtual const IProp* getMethodProp(const int8 mailBoxType, PyObject* attr);
    
    bool checkMethodArgs(IObject* object, const IProp* methodProp, PyObject* args);

	virtual const char* getModuleName() const { return _moduleName.c_str(); }
    virtual PyObject* createPyObject(const uint64 entityId);
    virtual void initializeEntity(PyObject* object, PyObject* dictData);
    virtual void setDefaultCellData(PyObject* dataDict);
    virtual void addCellDataToStream(PyObject* object, PyObject* cellDataDict, sl::IBMap& dataStream);

private:
	bool loadAllDefDescriptions(const char* moduleName, const sl::ISLXmlNode& root);
	bool loadDefPropertys(const char* moduleName, const sl::ISLXmlNode& root);
	bool loadDefCellMethods(const char* moduleName, const sl::ISLXmlNode& root);
	bool loadDefBaseMethods(const char* moduleName, const sl::ISLXmlNode& root);
	bool loadDefClientMethods(const char* moduleName, const sl::ISLXmlNode& root);
	bool loadDefMethods(const char* moduleName, const int8 type, const sl::ISLXmlNode& root);

	const IProp* getProp(PyObject* attr);
    bool appendObjectProp(PropDefInfo* layout, bool isMethod = false, bool isTemp = false);
    
    void createNameSpace(PyObject* object, PyObject* dictData);
    void initializeScript(PyObject* object);

private:
	typedef std::unordered_map<std::string, EntityDataFlags> ENTITY_FLAGS_MAP;
	//�ű����
    PyTypeObject*						_scriptType;
	sl::SLString<MAX_OBJECT_NAME_LEN>	_moduleName;
	static ENTITY_FLAGS_MAP				s_entityFlagMapping;
	bool								_hasBase;
	bool								_hasCell;
	bool								_hasClient;
	std::vector<PropDefInfo*>			_propsDefInfo;
    std::vector<PropDefInfo*>           _methodsDefInfo;
	//�����ֵ�
	PyObject*							_propDict;
    PyObject*                           _cellPropDict;
    //method Dict:
    PyObject*                           _cellMethodDict;
    PyObject*                           _baseMethodDict;
    PyObject*                           _clientMethodDict;
    RemoteEntityMethod*                 _remoteEntityMethod;
};
#endif
