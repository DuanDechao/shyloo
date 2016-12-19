#include "slnet.h"
#include "slmulti_sys.h"
#include "slxml_reader.h"
using namespace sl;
using namespace sl::network;

class testSessionFactory: public ISLSessionFactory
{
public:
	virtual ~testSessionFactory(){}
	virtual ISLSession* SLAPI createSession(ISLChannel* poChannel)
	{
		return nullptr;
	}
};

class testSession: public ISLSession
{
public:
	testSession(){}
	virtual ~testSession(){}
	virtual void SLAPI setChannel(ISLChannel* poChannel){}

	virtual void SLAPI onEstablish(void) {}

	virtual void SLAPI onTerminate(void) {}

	virtual void SLAPI release(void) {}

	virtual void SLAPI onRecv(const char* pBuf, uint32 dwLen) {}
};
int main()
{

	/*ISLNet* pNetModule = getSLNetModule();

	ISLConnector * pConnector = pNetModule->createConnector();
	ISLSession* pSession = new testSession();
	pConnector->setSession(pSession);
	pConnector->setBufferSize(1024, 1024);
	pConnector->connect("127.0.0.1", 7018);
	while(true){};*/
	XmlReader reader;
	if (!reader.loadXml("./test.xml")){
		SLASSERT(false, "can't find xml file");
		return -1;
	}

	ISLXmlNode& module = reader.root();
	ISLXmlNode& subModule = module["module"];
	const char* pName = subModule[0].getAttributeString("name");
	return 0;
}