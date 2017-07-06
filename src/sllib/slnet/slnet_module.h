#ifndef SL_LIB_NET_NET_MODULE_H
#define SL_LIB_NET_NET_MODULE_H
#include "slnet.h"
#include "slsingleton.h"
#include "sllistener.h"
#include "slconnector.h"
#include "slnetwork_interface.h"
#include "slevent_dispatcher.h"

namespace sl{
namespace network{
class CSLNetModule: public ISLNet, public CSingleton<CSLNetModule>{
public:
	CSLNetModule();
	virtual ~CSLNetModule();

	virtual ISLListener* SLAPI createListener();
	virtual ISLConnector* SLAPI createConnector();
	virtual bool SLAPI run(int64 overtime);
	virtual void SLAPI release();

	inline NetworkInterface* getNetworkInterface() {return _networkInterface;}

private:
	EventDispatcher					_dispatcher;
	NetworkInterface*				_networkInterface;
	std::vector<CSLListener*>		_listenerVec;
	std::vector<CSLConnector*>		_connectorVec;
};
}
}

#endif
