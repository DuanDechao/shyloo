#define SL_DLL_EXPORT
#include "slnet_module.h"
#include "slnet.h"
namespace sl{
SL_SINGLETON_INIT(network::CSLNetModule);
namespace network{

extern "C" SL_DLL_API ISLNet* SLAPI getSLNetModule(){
	CSLNetModule* g_netModulePtr = CSLNetModule::getSingletonPtr();
	if(g_netModulePtr == NULL)
		g_netModulePtr = NEW CSLNetModule();
	return CSLNetModule::getSingletonPtr();
}

CSLNetModule::CSLNetModule()
	:_dispatcher(),
	 _networkInterface(NULL),
	 _listenerVec(),
	 _connectorVec()
{
	_networkInterface = new NetworkInterface(&_dispatcher);
}

CSLNetModule::~CSLNetModule(){
	int32 size = _connectorVec.size();
	for (int32 i = 0; i < size; i++){
		if (_connectorVec[i])
			_connectorVec[i]->release();
	}

	size = _listenerVec.size();
	for (int32 i = 0; i < size; i++){
		if (_listenerVec[i])
			_listenerVec[i]->release();
	}

	if (nullptr != _networkInterface)
		DEL _networkInterface;
}

void CSLNetModule::release(){
	DEL this;
}

ISLListener* CSLNetModule::createListener(){
	CSLListener* poListener = new CSLListener();
	if (nullptr != poListener)
		_listenerVec.push_back(poListener);

	return poListener;
}

ISLConnector* CSLNetModule::createConnector(){
	CSLConnector* poConnector = new CSLConnector();
	if (nullptr != poConnector)
		_connectorVec.push_back(poConnector);

	return poConnector;
}

bool CSLNetModule::run(int64 overtime){
	int64 tick = sl::getTimeMilliSecond();
	_networkInterface->checkDestroyChannel();
	
	int64 netOverTime = overtime - sl::getTimeMilliSecond() + tick;
	netOverTime = netOverTime >= 0 ? netOverTime : 0;
	_dispatcher.processOnce(netOverTime);
	
	return true;
}

}
}