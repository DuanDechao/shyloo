#include "slnet.h"
#include "slmulti_sys.h"
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

	ISLNet* pNetModule = getSLNetModule();
	ISLListener* pListener = pNetModule->createListener();
	testSessionFactory sessionFactory;
	pListener->setSessionFactory(&sessionFactory);
	pListener->setBufferSize(1024, 1024);
	pListener->start("127.0.0.1", 7018);

	Sleep(5000);
	ISLConnector * pConnector = pNetModule->createConnector();
	ISLSession* pSession = new testSession();
	pConnector->setSession(pSession);
	pConnector->setBufferSize(1024, 1024);
	pConnector->connect("127.0.0.1", 7018);

	pNetModule->run(20);
	pListener->release();
	return 0;
}