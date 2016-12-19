#include "slnet.h"
#include "slmulti_sys.h"
#include "slxml_reader.h"
using namespace sl;
int main()
{
	XmlReader reader;
	if (!reader.loadXml("D:/shyloo/build/windows/Debug/test.xml")){
		SLASSERT(false, "can't find xml file");
		return -1;
	}

	ISLXmlNode& module = reader.root();
	ISLXmlNode& subModule = module["module"];
	const char* pName = subModule[0].getAttributeString("name");
	return 0;
}