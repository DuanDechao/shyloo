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

ISLListener* CSLNetModule::createListener()
{
	CSLListener* poListener = new CSLListener();
	return poListener;
}

}
}