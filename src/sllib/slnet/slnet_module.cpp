#include "slnet_module.h"
#include "slnet.h"
namespace sl
{
SL_SINGLETON_INIT(network::CSLNetModule);
namespace network
{

ISLNet* SLNetGetModule()
{
	return CSLNetModule::getSingletonPtr();
}

ISLListener* CSLNetModule::createListener()
{
	CSLListener* poListener = new CSLListener();
	return poListener;
}

}
}