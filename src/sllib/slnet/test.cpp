#include "slnet.h"
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
int main()
{

	ISLNet* pNetModule = getSLNetModule();
	ISLListener* pListener = pNetModule->createListener();
	testSessionFactory sessionFactory;
	pListener->setSessionFactory(&sessionFactory);
	pListener->setBufferSize(1024, 1024);
	pListener->start("127.0.0.1", 7011);
	while(true){};
	return 0;
}