#include "ScriptDefModule.h"
#include "slxml_reader.h"
#include "slfile_utils.h"
#include "IDCCenter.h"
#include "DataTypeMgr.h"
#include "NodeDefine.h"
#include "slstring_utils.h"
#include "EntityDef.h"
ScriptDefModule::ENTITY_FLAGS_MAP	ScriptDefModule::_entityFlagMapping;
ScriptDefModule::ScriptDefModule(const char* moduleName, ScriptDefModule* parentModule)
	:_moduleName(moduleName),
	_propDict(PyDict_New())
{
	loadParentModule(parentModule);
}

ScriptDefModule::~ScriptDefModule(){
	Py_DECREF(_propDict);
}

bool ScriptDefModule::initialize(){
	_entityFlagMapping["CELL_PUBLIC"] = ED_FLAG_CELL_PUBLIC;
	_entityFlagMapping["CELL_PRIVATE"] = ED_FLAG_CELL_PRIVATE;
	_entityFlagMapping["ALL_CLIENTS"] = ED_FLAG_ALL_CLIENTS;
	_entityFlagMapping["CELL_PUBLIC_AND_OWN"] = ED_FLAG_CELL_PUBLIC_AND_OWN;
	_entityFlagMapping["BASE_AND_CLIENT"] = ED_FLAG_BASE_AND_CLIENT;
	_entityFlagMapping["BASE"] = ED_FLAG_BASE;
	_entityFlagMapping["OTHER_CLIENTS"] = ED_FLAG_OTHER_CLIENTS;
	_entityFlagMapping["OWN_CLIENT"] = ED_FLAG_OWN_CLIENT;
	return true;
}

bool ScriptDefModule::loadParentModule(ScriptDefModule* parentModule){
	if (parentModule){
		const std::vector<PropLayout*>& parentLayouts = parentModule->propLayouts();
		
		for (auto layout : parentLayouts){
			_layouts.push_back(layout);
			appendObjectProp(layout);
		}

		if (parentModule->hasBase())
			_hasBase = true;

		if (parentModule->hasCell())
			_hasCell = true;

		if (parentModule->hasClient())
			_hasClient = true;
	}

	return true;
}

bool ScriptDefModule::loadFrom(const sl::ISLXmlNode& root){
	if (!loadAllDefDescriptions(_moduleName.c_str(), root)){
		SLASSERT(false, "load def description error");
		return false;
	}
	return true;
}


bool ScriptDefModule::loadAllDefDescriptions(const char* moduleName, const sl::ISLXmlNode& root){
	//加载属性描述
	if (!root.subNodeExist("Properties") || !loadDefPropertys(moduleName, root["Properties"][0]))
		return false;

	//加载cell方法描述
	if (root.subNodeExist("CellMethods")){
		if (!loadDefCellMethods(moduleName, root["CellMethods"][0]))
			return false;
	}

	//加载base方法描述
	if (root.subNodeExist("BaseMethods")){
		if (!loadDefBaseMethods(moduleName, root["BaseMethods"][0]))
			return false;
	}

	//加载client方法描述
	if (root.subNodeExist("ClientMethods")){
		if (!loadDefClientMethods(moduleName, root["ClientMethods"][0]))
			return false;
	}

	return true;
}

bool ScriptDefModule::loadDefPropertys(const char* moduleName, const sl::ISLXmlNode& root){
	std::vector<sl::ISLXmlNode*> allPropsConf = root.getAllChilds();
	for (auto propConf : allPropsConf){
		const char* propName = propConf->value();
		if (!propConf->subNodeExist("Flags")){
			SLASSERT(false, "prop %s has not Flags attribute", propName);
			return false;
		}
		std::string strFlags = (*propConf)["Flags"][0].text();
		std::transform(strFlags.begin(), strFlags.end(), strFlags.begin(), toupper);
		auto flagItor = _entityFlagMapping.find(strFlags);
		if (flagItor == _entityFlagMapping.end()){
			SLASSERT(false, "load def property: not find flags %s of prop:%s on module:%s", strFlags.c_str(), propName, moduleName);
			return false;
		}

		uint32 flags = flagItor->second;
		uint32 hasBaseFlags = flags & ENTITY_BASE_DATA_FLAGS;
		if (hasBaseFlags > 0)
			_hasBase = true;

		uint32 hasCellFlags = flags & ENTITY_CELL_DATA_FLAGS;
		if (hasCellFlags > 0)
			_hasCell = true;

		uint32 hasClientFlags = flags & ENTITY_CLIENT_DATA_FLAGS;
		if (hasClientFlags > 0)
			_hasClient = true;

		if (hasBaseFlags <= 0 && hasCellFlags <= 0){
			SLASSERT(false, "load Def property error: flags[%s] of prop[%s] on module[%s] not both on cell and base", strFlags.c_str(), propName, moduleName);
			return false;
		}

		bool isPersistent = false;
		if (propConf->subNodeExist("Persistent")){
			std::string strPersistent = (*propConf)["Persistent"][0].text();
			std::transform(strPersistent.begin(), strPersistent.end(), strPersistent.begin(), tolower);
			if (strPersistent == "true")
				flags |= ED_FLAG1_PERSISTENT;
		}

		if (!propConf->subNodeExist("Type")){
			SLASSERT(false, "loadDefProperty error: not found Type of prop[%s] on module[%s]", propName, moduleName);
			return false;
		}

		const char* strType = (*propConf)["Type"][0].text();
		DATATYPE_UID dateType = DataTypeMgr::getDataTypeUid(strType);
		if (propConf->subNodeExist("Index")){

		}

		bool isIdentifier = false;
		if (propConf->subNodeExist("Identifier")){
			std::string strIdentifier = (*propConf)["Identifier"][0].text();
			std::transform(strIdentifier.begin(), strIdentifier.end(), strIdentifier.begin(), tolower);
			if (strIdentifier == "true")
				flags |= ED_FLAG1_IDENTIFIER;
		}

		PropLayout* layout = NEW PropLayout();
		layout->_name = propName;
		layout->_size = sizeof(uint64);
		layout->_index = 0;
		layout->_flags = flags;
		layout->_type = dateType;
		_layouts.push_back(layout);
		
		appendObjectProp(layout);
	}

	return true;
}

bool ScriptDefModule::loadDefCellMethods(const char* moduleName, const sl::ISLXmlNode& root){
	return loadDefMethods(moduleName, NodeType::SCENE, root);
}

bool ScriptDefModule::loadDefBaseMethods(const char* moduleName, const sl::ISLXmlNode& root){
	return loadDefMethods(moduleName, NodeType::LOGIC, root);
}

bool ScriptDefModule::loadDefClientMethods(const char* moduleName, const sl::ISLXmlNode& root){
	return loadDefMethods(moduleName, NodeType::GATE, root);
}

bool ScriptDefModule::loadDefMethods(const char* moduleName, const int8 type, const sl::ISLXmlNode& root){
	std::vector<sl::ISLXmlNode*> allMethodsConf = root.getAllChilds();
	for (auto methodConf : allMethodsConf){
		const char* methodName = methodConf->value();
		bool isExposed = false;
		if (methodConf->subNodeExist("Exposed"))
			isExposed = true;

		std::vector<uint8> dataTypes;
		if (methodConf->subNodeExist("Arg")){
			const sl::ISLXmlNode& args = (*methodConf)["Arg"];
			for (int32 i = 0; i < args.count(); i++){
				const char* strType = args[i].text();
				DATATYPE_UID dateType = DataTypeMgr::getDataTypeUid(strType);
				dataTypes.push_back(dateType);
			}
		}

		SLMODULE(ObjectMgr)->appendObjectMethod(moduleName, methodName, type, 0, 0, dataTypes);
	}

	return true;
}

bool ScriptDefModule::appendObjectProp(PropLayout* layout){
	const IProp* prop = SLMODULE(ObjectMgr)->appendObjectProp(_moduleName.c_str(), layout->_name.c_str(), layout->_type, layout->_size, layout->_flags, layout->_index);
	if (!prop){
		SLASSERT(false, "wtf");
		return false;
	}

	std::string propName = layout->_name.c_str();
	PyObject* pykey = PyUnicode_FromString(propName.c_str());
	PyObject* pyPropPtr = PyLong_FromVoidPtr(const_cast<IProp*>(prop));
	if (PyErr_Occurred()){
		PyErr_Clear();
		PyErr_Format(PyExc_TypeError, "createObject failed");
		PyErr_PrintEx(0);

		S_RELEASE(pyPropPtr);
		S_RELEASE(pykey);
		return false;
	}

	PyDict_SetItem(_propDict, pykey, pyPropPtr);
	Py_DECREF(pykey);
	Py_DECREF(pyPropPtr);
	return true;
}

PyObject* ScriptDefModule::scriptGetObjectAttribute(PyObject* object, PyObject* attr){
	const IProp* prop = getProp(attr);
	if (!prop){
		return nullptr;
	}

	IObject* innerObject = getMMObject(object);
	if (!innerObject){
		SLASSERT(false, "wtf");
		return nullptr;
	}
	return DataTypeMgr::getPyAttrValue(innerObject, prop);
}

int32 ScriptDefModule::scriptSetObjectAttribute(PyObject* object, PyObject* attr, PyObject* value){
	const IProp* prop = getProp(attr);
	if (!prop){
		return -1;
	}

	IObject* innerObject = getMMObject(object);
	if (!innerObject){
		SLASSERT(false, "wtf");
		return -2;
	}

	return DataTypeMgr::setPyAttrValue(innerObject, prop, value);
}

PyObject* ScriptDefModule::createObject(void){
	PyObject* pObject = PyType_GenericAlloc(_scriptType, 0);
	if (pObject == NULL){
		PyErr_Print();
		SLASSERT(false, "wtf");
	}

	IObject* object = CREATE_OBJECT(SLMODULE(ObjectMgr), _moduleName.c_str());

	PyObject* pyObjectPtr = PyLong_FromVoidPtr(object);

	if (PyErr_Occurred()){
		PyErr_Clear();
		PyErr_Format(PyExc_TypeError, "createObject failed");
		PyErr_PrintEx(0);

		S_RELEASE(pyObjectPtr);
		return NULL;
	}

	PyObject_SetAttrString(static_cast<PyObject*>(pObject), "_innerObject_", pyObjectPtr);

	return pObject;
}

IObject* ScriptDefModule::getMMObject(PyObject* object){
	PyObject* innerObjectPtr = PyObject_GetAttrString(object, "_innerObject_");
	IObject* innerObject = (IObject*)PyLong_AsVoidPtr(innerObjectPtr);
	if (!innerObject){
		SLASSERT(false, "wtf");
		return nullptr;
	}
	return innerObject;
}

const IProp* ScriptDefModule::getProp(PyObject* attr){
	wchar_t*wideCharString = PyUnicode_AsWideCharString(attr, NULL);
	char* valStr = sl::CStringUtils::wchar2char(wideCharString);
	PyMem_Free(wideCharString);

	PyObject* pyVal = PyDict_GetItem(_propDict, attr);
	if (!pyVal){
		return nullptr;
	}
	return (const IProp*)PyLong_AsVoidPtr(pyVal);
}

