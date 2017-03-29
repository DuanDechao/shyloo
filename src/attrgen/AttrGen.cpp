#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <iomanip>
#include "slmulti_sys.h"
#include "slxml_reader.h"
#include "slfile_utils.h"
#include "sltools.h"
using namespace std;
void propEnumGen(const char* attrFileName, std::set<std::string>& propNames){
	std::ofstream attrFile(attrFileName);
	if (!attrFile){
		SLASSERT(false, "can not open file %s", attrFileName);
		ECHO_ERROR("can not open file %s", attrFileName);
		return;
	}
	attrFile << "#ifndef __ATTR_H__" << endl;
	attrFile << "#define __ATTR_H__" << endl << endl;
	attrFile << "#ifdef ATTR_EXPORT" << endl;
	attrFile << "#define ATTR_API __declspec (dllexport)" << endl;
	attrFile << "#else" << endl;
	attrFile << "#define ATTR_API __declspec (dllimport)" << endl;
	attrFile << "#endif" << endl <<endl;

	attrFile << "extern \"C\" ATTR_API struct attr_def{" << endl;
	std::set<std::string>::iterator itor = propNames.begin();
	std::set<std::string>::iterator itorEnd = propNames.end();
	while (itor != itorEnd){
		attrFile << "	const IProp* " << *itor << "		= nullptr;" << endl;
		++itor;
	}
	attrFile << "}" << endl << endl;
	attrFile << "#endif" << endl;
	attrFile.close();                   //¹Ø±ÕÎÄ¼þ
}

int main(){
	char dccenterPath[256] = { 0 };
	SafeSprintf(dccenterPath, sizeof(dccenterPath), "%s/envir/dccenter/", sl::getAppPath());

	
	std::vector<std::string> files;
	sl::ListFileInDirection(dccenterPath, ".xml", [&files](const char * name, const char * path) {
		files.push_back(path);
	});

	std::set<std::string> propNames;
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
				SLASSERT(false, "has duplicate prop name %s", name);
				ECHO_ERROR("has duplicate prop name %s", name);
				return -1;
			}
			propNames.insert(name);
		}
	}

	char attrFile[256] = { 0 };
	SafeSprintf(attrFile, sizeof(attrFile), "%s/../../../src/logic/define/Attr.h", sl::getAppPath());
	propEnumGen(attrFile, propNames);
	printf("genentor attr define file success %s\n", attrFile);
	system("pause");
	return 0;
}