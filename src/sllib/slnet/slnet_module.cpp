#include "slnet_module.h"
#include "slnet.h"
#include "slconnector.h"
namespace sl
{
SL_SINGLETON_INIT(network::CSLNetModule);
namespace network
{

ISLNet* SLAPI getSLNetModule()
{
	CSLNetModule* g_netModulePtr = CSLNetModule::getSingletonPtr();
	if(g_netModulePtr == NULL)
		g_netModulePtr = new CSLNetModule();
	return CSLNetModule::getSingletonPtr();
}

CSLNetModule::CSLNetModule()
{
	m_networkInterface = new NetworkInterface(&m_dispatcher);
}

CSLNetModule::~CSLNetModule(){
	delete m_networkInterface;
}

ISLListener* CSLNetModule::createListener()
{
	CSLListener* poListener = new CSLListener();
	return poListener;
}

ISLConnector* CSLNetModule::createConnector()
{
	CSLConnector* poConnector = new CSLConnector();
	return poConnector;
}

bool CSLNetModule::run(int64 overtime)
{
	m_dispatcher.processOnce();
	return true;
}

}
}