#include "slnet_module.h"
#include "slnet.h"
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
	:m_dispatcher(),
	 m_networkInterface(NULL),
	 m_listenerVec(),
	 m_connectorVec()
{
	m_networkInterface = new NetworkInterface(&m_dispatcher);
}

CSLNetModule::~CSLNetModule(){
	if (nullptr != m_networkInterface)
		delete m_networkInterface;
}

ISLListener* CSLNetModule::createListener()
{
	CSLListener* poListener = new CSLListener();
	if (nullptr != poListener)
		m_listenerVec.push_back(poListener);

	return poListener;
}

ISLConnector* CSLNetModule::createConnector()
{
	CSLConnector* poConnector = new CSLConnector();
	if (nullptr != poConnector)
		m_connectorVec.push_back(poConnector);

	return poConnector;
}

bool CSLNetModule::run(int64 overtime)
{
	m_dispatcher.processOnce();
	m_networkInterface->checkDestroyChannel();
	return true;
}

}
}