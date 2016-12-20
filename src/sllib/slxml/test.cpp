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

	ISLXmlNode& modules = reader.root()["module"];
	const char* pName = nullptr;
	for (int32 i = 0; i < modules.count(); i++)
	{
		pName = modules[i].getAttributeString("name");
	}
	return 0;
}