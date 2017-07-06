#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <iomanip>
#include <unordered_map>
#include "slmulti_sys.h"
#include "slxml_reader.h"
#include "slfile_utils.h"
#include "sltools.h"
using namespace std;
void propEnumGen(const char* attrFileName, std::set<std::string>& propNames, std::set<std::string>& tempPropNames, std::unordered_map<std::string, std::vector<std::string>>& tablesInfo, std::unordered_map<std::string, std::vector<std::string>>& staticTablesInfo){
	std::ofstream attrFile(attrFileName);
	if (!attrFile){
		SLASSERT(false, "can not open file %s", attrFileName);
		ECHO_ERROR("can not open file %s", attrFileName);
		return;
	}
	attrFile << "#ifndef __ATTR_H__" << endl;
	attrFile << "#define __ATTR_H__" << endl;
	attrFile << "#include \"slmulti_sys.h\" " << endl << endl;
	attrFile << "#ifdef SL_OS_WINDOWS" << endl;
	attrFile << "#ifdef ATTR_EXPORT" << endl;
	attrFile << "#define ATTR_API __declspec (dllexport)" << endl;
	attrFile << "#else" << endl;
	attrFile << "#define ATTR_API __declspec (dllimport)" << endl;
	attrFile << "#endif" << endl;
	attrFile << "#else" << endl;
	attrFile << "#define ATTR_API" << endl;
	attrFile << "#endif" << endl <<endl;

	attrFile << "class IProp;" << endl << endl;


	char objectPropDefine[256] = { 0 };
	SafeSprintf(objectPropDefine, sizeof(objectPropDefine), "%s/envir/object.xml", sl::getAppPath());
	sl::XmlReader conf;
	if (!conf.loadXml(objectPropDefine)){
		return;
	}

	attrFile << "namespace prop_def {" << endl;
	attrFile << "	enum {" << endl;
	if (conf.root().subNodeExist("prop")){
		const sl::ISLXmlNode& props = conf.root()["prop"];
		for (int32 i = 0; i < props.count(); i++){
			const char* name = props[i].getAttributeString("name");
			attrFile << "		" << props[i].getAttributeString("name") << " = " << (int32)(1 << i)  << "," << endl;
		}
	}
	attrFile << "	};" << endl;
	attrFile << "}" << endl << endl;

	attrFile << "struct ATTR_API attr_def{" << endl;
	std::set<std::string>::iterator itor = propNames.begin();
	std::set<std::string>::iterator itorEnd = propNames.end();
	while (itor != itorEnd){
		attrFile << "	static const IProp* " << *itor << ";" << endl;
		++itor;
	}
	attrFile << "};" << endl << endl;

	attrFile << "struct ATTR_API OCTempProp{" << endl;
	std::set<std::string>::iterator tempItor = tempPropNames.begin();
	std::set<std::string>::iterator tempItorEnd = tempPropNames.end();
	while (tempItor != tempItorEnd){
		attrFile << "	static const IProp* " << *tempItor << ";" << endl;
		++tempItor;
	}
	attrFile << "};" << endl << endl;

	attrFile << "namespace OCTableMacro {" << endl;
	if (!tablesInfo.empty()){
		auto tableItor = tablesInfo.begin();
		for (; tableItor != tablesInfo.end(); ++tableItor){
			if (tableItor->second.empty())
				continue;
			attrFile << "	namespace " << tableItor->first << " {" << endl;
			attrFile << "		static int32 TABLE_NAME = " << sl::CalcStringUniqueId(tableItor->first.c_str()) << ";" << endl;
			attrFile << "		enum {" << endl;
			attrFile << "			OCTM_START = 0," << endl;
			for (int32 i = 0; i < tableItor->second.size(); i++){
				if (i == 0)
					attrFile << "			" << tableItor->second[i] << " = OCTM_START," << endl;
				else
					attrFile << "			" << tableItor->second[i] << "," << endl;
			}
			attrFile << "			OCTM_END," << endl;
			attrFile << "		};" << endl;
			attrFile << "	}" << endl << endl;
		}
	}
	attrFile << "};" << endl << endl;

	attrFile << "namespace OCStaticTableMacro {" << endl;
	if (!staticTablesInfo.empty()){
		auto tableItor = staticTablesInfo.begin();
		for (; tableItor != staticTablesInfo.end(); ++tableItor){
			if (tableItor->second.empty())
				continue;
			attrFile << "	namespace " << tableItor->first << " {" << endl;
			attrFile << "		static const char* TABLE_NAME = \"" << tableItor->first.c_str() << "\";" << endl;
			attrFile << "		enum {" << endl;
			attrFile << "			OCTM_START = 0," << endl;
			for (int32 i = 0; i < tableItor->second.size(); i++){
				if (i == 0)
					attrFile << "			" << tableItor->second[i] << " = OCTM_START," << endl;
				else
					attrFile << "			" << tableItor->second[i] << "," << endl;
			}
			attrFile << "			OCTM_END," << endl;
			attrFile << "		};" << endl;
			attrFile << "	}" << endl << endl;
		}
	}
	attrFile << "};" << endl << endl;

	attrFile << "#endif" << endl;
	attrFile.close();                   //关闭文件
}

void attrGetterGen(const char* attrFileName, std::set<std::string>& propNames, std::set<std::string>& tempPropNames){
	std::ofstream attrFile(attrFileName);
	if (!attrFile){
		SLASSERT(false, "can not open file %s", attrFileName);
		ECHO_ERROR("can not open file %s", attrFileName);
		return;
	}
	attrFile << "#define ATTR_EXPORT" << endl;
	attrFile << "#include \"AttrGetter.h\"" << endl;
	attrFile << "#include \"Attr.h\"" << endl;
	attrFile << "#include \"IDCCenter.h\"" << endl << endl;

	attrFile << "void getAttrProp(IObjectMgr* objectMgr);" << endl << endl;

	attrFile << "bool AttrGetter::initialize(sl::api::IKernel * pKernel){" << endl;
	attrFile << "	_kernel = pKernel;" << endl;
	attrFile << "	FIND_MODULE(_objectMgr, ObjectMgr);" << endl << endl;
	attrFile << "	getAttrProp(_objectMgr);" << endl;
	attrFile << "	return true;" << endl;
	attrFile << "}" << endl << endl;

	attrFile << "bool AttrGetter::launched(sl::api::IKernel * pKernel){" << endl;
	attrFile << "	return true;" << endl;
	attrFile << "}" << endl << endl;

	attrFile << "bool AttrGetter::destory(sl::api::IKernel * pKernel){" << endl;
	attrFile << "	DEL this;" << endl;
	attrFile << "	return true;" << endl;
	attrFile << "}" << endl << endl;

	std::set<std::string>::iterator itor = propNames.begin();
	std::set<std::string>::iterator itorEnd = propNames.end();
	while (itor != itorEnd){
		attrFile << "const IProp* attr_def::" << *itor << " = nullptr;" << endl;
		++itor;
	}
	attrFile << endl;

	std::set<std::string>::iterator tempItor = tempPropNames.begin();
	std::set<std::string>::iterator tempItorEnd = tempPropNames.end();
	while (tempItor != tempItorEnd){
		attrFile << "const IProp* OCTempProp::" << *tempItor << " = nullptr;" << endl;
		++tempItor;
	}

	attrFile << endl;
	attrFile << "void getAttrProp(IObjectMgr* objectMgr){" << endl;
	itor = propNames.begin();
	while (itor != itorEnd){
		attrFile << "	attr_def::" << *itor << " = objectMgr->getPropByName(\"" << *itor << "\");" << endl;
		++itor;
	}

	attrFile << endl;
	
	tempItor = tempPropNames.begin();
	while (tempItor != tempItorEnd){
		attrFile << "	OCTempProp::" << *tempItor << " = objectMgr->getTempPropByName(\"" << *tempItor << "\");" << endl;
		++tempItor;
	}

	attrFile << "}" << endl << endl;
	attrFile.close();                   //关闭文件
}

int main(){
	char dccenterPath[256] = { 0 };
	SafeSprintf(dccenterPath, sizeof(dccenterPath), "%s/envir/dccenter/", sl::getAppPath());

	
	std::vector<std::string> files;
	sl::CFileUtils::ListFileInDirection(dccenterPath, ".xml", [&files](const char * name, const char * path) {
		files.push_back(path);
	});

	std::set<std::string> propNames;
	std::set<std::string> tempPropNames;
	std::unordered_map<std::string, std::vector<std::string>> tablesInfo;
	std::unordered_map<std::string, std::vector<std::string>> staticTablesInfo;
	for (int32 i = 0; i < (int32)files.size(); i++){
		sl::XmlReader propConf;
		if (!propConf.loadXml(files[i].c_str())){
			SLASSERT(false, "can not load file %s", files[i].c_str());
			ECHO_ERROR("can not load file %s", files[i].c_str());
			return -1;
		}
		const sl::ISLXmlNode& props = propConf.root()["prop"];
		for (int32 i = 0; i < props.count(); i++){
			std::string name = props[i].getAttributeString("name");
			if (propNames.find(name) != propNames.end()){
				continue;
			}
			propNames.insert(name);
		}

		if (propConf.root().subNodeExist("temp")){
			const sl::ISLXmlNode& tempProps = propConf.root()["temp"];
			for (int32 i = 0; i < tempProps.count(); i++){
				std::string name = tempProps[i].getAttributeString("name");
				if (tempPropNames.find(name) != tempPropNames.end()){
					continue;
				}
				tempPropNames.insert(name);
			}
		}

		if (propConf.root().subNodeExist("table")){
			const sl::ISLXmlNode& tables = propConf.root()["table"];
			for (int32 i = 0; i < tables.count(); i++){
				const char* tableName = tables[i].getAttributeString("name");
				if (tablesInfo.find(tableName) != tablesInfo.end())
					continue;

				const sl::ISLXmlNode& columns = tables[i]["column"];
				for (int32 j = 0; j < columns.count(); j++){
					const char* columnName = columns[j].getAttributeString("name");
					tablesInfo[tableName].push_back(columnName);
				}
			}
		}
		
	}

	char staticTableFile[256] = { 0 };
	SafeSprintf(staticTableFile, sizeof(staticTableFile), "%s/envir/object.xml", sl::getAppPath());
	sl::XmlReader staticTableConf;
	if (!staticTableConf.loadXml(staticTableFile)){
		SLASSERT(false, "can not load file %s", staticTableFile);
		ECHO_ERROR("can not load file %s", staticTableFile);
		return -1;
	}

	if (staticTableConf.root().subNodeExist("table")){
		const sl::ISLXmlNode& tables = staticTableConf.root()["table"];
		for (int32 i = 0; i < tables.count(); i++){
			const char* tableName = tables[i].getAttributeString("name");
			if (staticTablesInfo.find(tableName) != staticTablesInfo.end())
				continue;

			const sl::ISLXmlNode& columns = tables[i]["column"];
			for (int32 j = 0; j < columns.count(); j++){
				const char* columnName = columns[j].getAttributeString("name");
				staticTablesInfo[tableName].push_back(columnName);
			}
		}
	}

	char attrFile[256] = { 0 };
	SafeSprintf(attrFile, sizeof(attrFile), "%s/../../../src/logic/define/Attr.h", sl::getAppPath());
	propEnumGen(attrFile, propNames, tempPropNames, tablesInfo, staticTablesInfo);

	char attrGetterFile[256] = { 0 };
	SafeSprintf(attrGetterFile, sizeof(attrGetterFile), "%s/../../../src/logic/attrgetter/AttrGetter.cpp", sl::getAppPath());
	attrGetterGen(attrGetterFile, propNames, tempPropNames);
	printf("genentor attr define file success %s\n", attrFile);
	system("pause");
	return 0;
}