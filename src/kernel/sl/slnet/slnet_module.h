#ifndef SL_SLNET_MODULE_H
#define SL_SLNET_MODULE_H
#include "slnet.h"
#include "slsingleton.h"
#include "slmessage_handler.h"
#include "sllistener.h"
#include "slnetwork_interface.h"
#include "slevent_dispatcher.h"



namespace sl
{
using namespace network;
class CSLNetModule: public ISLNet, public CSingleton<CSLNetModule>
{
public:
	virtual ISLListener* SLAPI createListener();

	virtual void SLAPI registerExtMessageHandler(MessageID msgID, msgHandlerCB& handlerCB);
	virtual void SLAPI registerIntMessageHandler(MessageID msgID, msgHandlerCB& handlerCB);

	inline EventDispatcher* getEventDispatcher() {return &m_dispatcher;}
private:
	MessageHandlers					m_extMessageHandlers;
	MessageHandlers					m_intMessageHandlers;
	EventDispatcher					m_dispatcher;
};

}

#endif