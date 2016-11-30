#include "slnet.h"
using namespace sl;
using namespace sl::network;



int main()
{

	ISLNet* pNetModule = getSLNetModule();
	ISLListener* pListener = pNetModule->createListener();
	pListener->start("127.0.0.1", 7011);
	while(true){};
	return 0;
}