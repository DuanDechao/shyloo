#include "ObjectDefModule.h"
#include "slxml_reader.h"
#include "slfile_utils.h"
#include "IDCCenter.h"
#include "DataTypeMgr.h"
#include "NodeDefine.h"
#include "slstring_utils.h"
#include "ObjectDef.h"
#include "NodeProtocol.h"
//#include "IMmoServer.h"
#include "GameDefine.h"
#include "IHarbor.h"
#include "Attr.h"
#include "sllist.h"

ObjectDefModule::OBJECT_FLAGS_MAP ObjectDefModule::s_objectFlagMapping;
ObjectDefModule::ObjectDefModule(const char* moduleName, ObjectDefModule* parentModule)
	:_moduleName(moduleName),
    _hasCell(false),
    _hasBase(false),
    _hasClient(false)
{
	loadParentModule(parentModule);
}

ObjectDefModule::~ObjectDefModule(){
}

bool ObjectDefModule::initialize(){
	s_objectFlagMapping["CELL_PUBLIC"] = prop_def::ObjectDataFlag::CELL_PUBLIC;
	s_objectFlagMapping["CELL_PRIVATE"] = prop_def::ObjectDataFlag::CELL_PRIVATE;
	s_objectFlagMapping["ALL_CLIENTS"] = prop_def::ObjectDataFlag::ALL_CLIENTS;
	s_objectFlagMapping["CELL_PUBLIC_AND_OWN"] = prop_def::ObjectDataFlag::CELL_PUBLIC_AND_OWN;
	s_objectFlagMapping["BASE_AND_CLIENT"] = prop_def::ObjectDataFlag::BASE_AND_CLIENT;
	s_objectFlagMapping["BASE"] = prop_def::ObjectDataFlag::BASE;
	s_objectFlagMapping["OTHER_CLIENTS"] = prop_def::ObjectDataFlag::OTHER_CLIENTS;
	s_objectFlagMapping["OWN_CLIENT"] = prop_def::ObjectDataFlag::OWN_CLIENT;
	return true;
}

bool ObjectDefModule::loadParentModule(ObjectDefModule* parentModule){
	if (parentModule){
		const std::vector<PropDefInfo*>& parentPropsDefInfo = parentModule->propsDefInfo();
		
		for (auto defInfo : parentPropsDefInfo){
			_propsDefInfo.push_back(defInfo);
			appendObjectProp(defInfo);
		}

        const std::vector<PropDefInfo*>& parentMethodsDefInfo = parentModule->methodsDefInfo();
        for (auto defInfo : parentMethodsDefInfo){
            _methodsDefInfo.push_back(defInfo);
            appendObjectProp(defInfo, true);
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

bool ObjectDefModule::loadFrom(const sl::ISLXmlNode& root){
	if (!loadAllDefDescriptions(_moduleName.c_str(), root)){
		SLASSERT(false, "load def description error");
		return false;
	}
	return true;
}


bool ObjectDefModule::loadAllDefDescriptions(const char* moduleName, const sl::ISLXmlNode& root){
	//º”‘ÿ Ù–‘√Ë ˆ
	if (!root.subNodeExist("Properties") || !loadDefPropertys(moduleName, root["Properties"][0]))
		return false;

	//º”‘ÿcell∑Ω∑®√Ë ˆ
	if (root.subNodeExist("CellMethods")){
		if (!loadDefCellMethods(moduleName, root["CellMethods"][0]))
			return false;
	}

	//º”‘ÿbase∑Ω∑®√Ë ˆ
	if (root.subNodeExist("BaseMethods")){
		if (!loadDefBaseMethods(moduleName, root["BaseMethods"][0]))
			return false;
	}

	//º”‘ÿclient∑Ω∑®√Ë ˆ
	if (root.subNodeExist("ClientMethods")){
		if (!loadDefClientMethods(moduleName, root["ClientMethods"][0]))
			return false;
	}

	return true;
}

bool ObjectDefModule::loadDefPropertys(const char* moduleName, const sl::ISLXmlNode& root){
	static int32 g_propIndexAuto = 1;
	std::vector<sl::ISLXmlNode*> allPropsConf = root.getAllChilds();
	for (auto propConf : allPropsConf){
		const char* propName = propConf->value();
		if (!propConf->subNodeExist("Flags")){
			ECHO_ERROR(false, "prop %s has not Flags attribute", propName);
			return false;
		}
        
        uint32 flags = prop_def::ObjectPropType::PROP;
		std::string strFlags = (*propConf)["Flags"][0].text();
        sl::CStringUtils::MakeUpper(strFlags);
        auto flagItor = s_objectFlagMapping.find(strFlags);
		if (flagItor == s_objectFlagMapping.end()){
			SLASSERT(false, "load def property: not find flags %s of prop:%s on module:%s", strFlags.c_str(), propName, moduleName);
			return false;
		}
        
		flags |= flagItor->second;
		uint32 hasBaseFlags = flags & prop_def::OBJECT_BASE_DATA_FLAGS;
		if (hasBaseFlags > 0)
			_hasBase = true;

		uint32 hasCellFlags = flags & prop_def::OBJECT_CELL_DATA_FLAGS;
		if (hasCellFlags > 0)
			_hasCell = true;

		uint32 hasClientFlags = flags & prop_def::OBJECT_CLIENT_DATA_FLAGS;
		if (hasClientFlags > 0)
			_hasClient = true;

		if (hasBaseFlags <= 0 && hasCellFlags <= 0){
			ECHO_ERROR(false, "load Def property error: flags[%s] of prop[%s] on module[%s] not both on cell and base", strFlags.c_str(), propName, moduleName);
			return false;
		}

		if (propConf->subNodeExist("Persistent")){
			std::string strPersistent = (*propConf)["Persistent"][0].text();
            sl::CStringUtils::MakeLower(strPersistent);
            if (strPersistent == "true")
				flags |= prop_def::ObjectDBFlag::persistent;
		}

		if (!propConf->subNodeExist("Type")){
			ECHO_ERROR(false, "loadDefProperty error: not found Type of prop[%s] on module[%s]", propName, moduleName);
			return false;
		}
		const char* strType = (*propConf)["Type"][0].text();
        if (propConf->subNodeExist("Index")){
			std::string strIndex = (*propConf)["Index"][0].text();
            sl::CStringUtils::MakeLower(strIndex);
            if (strIndex == "true")
				flags |= prop_def::ObjectDBFlag::index;
		}

		if (propConf->subNodeExist("Identifier")){
			std::string strIdentifier = (*propConf)["Identifier"][0].text();
            sl::CStringUtils::MakeLower(strIdentifier);
            if (strIdentifier == "true")
				flags |= prop_def::ObjectDBFlag::identifier;
		}

		int32 propIndex = 0;
		if(propConf->subNodeExist("Utype"))
			propIndex = sl::CStringUtils::StringAsInt32((*propConf)["Utype"][0].text());
		else
			propIndex = g_propIndexAuto++;

		const char* defaultVal = "";
		if(propConf->subNodeExist("Default"))
			defaultVal = (*propConf)["Default"][0].text();

	    int32 nodeType = SLMODULE(Harbor)->getNodeType();
        if(nodeType == NodeType::SCENE && !hasCellFlags)
            continue;

		//if(nodeType == NodeType::LOGIC && !hasBaseFlags)
		//	continue;

		IDataType* dataType = DataTypeMgr::getDataType((*propConf)["Type"][0]);
		if(!dataType){
			printf("error!! has no DataType[%s]!!", strType);
			return false;
		}
		PropDefInfo* info = NEW PropDefInfo();
		info->_name = propName;
		info->_size = dataType->getSize();
		info->_index = propIndex;
		info->_flags = flags;
		info->_type = DataTypeMgr::getType(strType);
        info->_extra = dataType;
		info->_defaultVal = defaultVal;
		_propsDefInfo.push_back(info);
		appendObjectProp(info);
	}

	return true;
}

bool ObjectDefModule::loadDefCellMethods(const char* moduleName, const sl::ISLXmlNode& root){
	return loadDefMethods(moduleName, RemoteMethodType::RMT_CELL, root);
}

bool ObjectDefModule::loadDefBaseMethods(const char* moduleName, const sl::ISLXmlNode& root){
	return loadDefMethods(moduleName, RemoteMethodType::RMT_BASE, root);
}

bool ObjectDefModule::loadDefClientMethods(const char* moduleName, const sl::ISLXmlNode& root){
	return loadDefMethods(moduleName, RemoteMethodType::RMT_CLIENT, root);
}

bool ObjectDefModule::loadDefMethods(const char* moduleName, const int8 type, const sl::ISLXmlNode& root){
	static int32 g_methodIndexAuto = 1;
	std::vector<sl::ISLXmlNode*> allMethodsConf = root.getAllChilds();
	for (auto methodConf : allMethodsConf){
		const char* methodName = methodConf->value();
		bool isExposed = false;
		if (methodConf->subNodeExist("Exposed"))
			isExposed = true;

		int32 methodIndex = 0;
		if(methodConf->subNodeExist("Utype"))
			methodIndex = sl::CStringUtils::StringAsInt32((*methodConf)["Utype"][0].text());
		else
			methodIndex = g_methodIndexAuto++;

		IDataType** pArgsList = nullptr;
		int32 argsCount = 0;
		if (methodConf->subNodeExist("Arg")){
			const sl::ISLXmlNode& args = (*methodConf)["Arg"];
			argsCount = args.count();
			if(argsCount <= 0)
				continue;

			pArgsList = NEW IDataType*[argsCount];
			for (int32 i = 0; i < argsCount; i++){
				const char* strType = args[i].text();
				IDataType* arg = DataTypeMgr::getDataType(strType);
				if(!arg){
					printf("Module[%s]'s method[%s] Args DataType[%s] is null!\n", moduleName, methodName, strType);
					return false;
				}
				pArgsList[i] = arg;
            }
        }
       
        uint32 flags = prop_def::ObjectPropType::METHOD;        
	
        PropDefInfo* info = NEW PropDefInfo();
        info->_name = methodName;

        //øøøøøøøøøøøøøø(uint64)
        info->_size = argsCount;
        info->_index = methodIndex;
        info->_flags = flags;
        info->_type = type;
        info->_extra = pArgsList;
        _methodsDefInfo.push_back(info);

        appendObjectProp(info, true, true);    
	}

	return true;
}

bool ObjectDefModule::appendObjectProp(PropDefInfo* defInfo, bool isMethod, bool isTemp){
	const IProp* prop = isTemp ? SLMODULE(ObjectMgr)->appendObjectTempProp(_moduleName.c_str(), defInfo->_name.c_str(), defInfo->_type, defInfo->_size, defInfo->_flags, defInfo->_index, defInfo->_extra, defInfo->_defaultVal.c_str()) : SLMODULE(ObjectMgr)->appendObjectProp(_moduleName.c_str(), defInfo->_name.c_str(), defInfo->_type, defInfo->_size, defInfo->_flags, defInfo->_index, defInfo->_extra, defInfo->_defaultVal.c_str());
	if (!prop){
		SLASSERT(false, "wtf");
		return false;
	}
    
    PROPS_MAP* propMap = &_props;
    PROPS_IDMAP* idPropMap = &_idToProps;
    if(isMethod){
        switch(defInfo->_type){
        case RemoteMethodType::RMT_CLIENT: propMap = &_clientMethods; idPropMap = &_idToClientMethods; break;
        case RemoteMethodType::RMT_CELL: propMap = &_cellMethods; idPropMap = &_idToCellMethods; break;
        case RemoteMethodType::RMT_BASE: propMap = &_baseMethods; idPropMap = &_idToBaseMethods; break;
        default: break;
        }
    }

    (*propMap)[defInfo->_name.c_str()] = prop;
    (*propMap)[defInfo->_name.c_str()] = prop;
    (*idPropMap)[defInfo->_index] = prop;
    (*idPropMap)[defInfo->_index] = prop;
    return true;
}

const IProp* ObjectDefModule::getPropByUid(const int32 uid) const{
	auto itor = _idToProps.find(uid);
	if(itor == _idToProps.end())
		return nullptr;

	return itor->second;
}
