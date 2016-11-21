#include "slnet_module.h"
namespace sl
{
SL_SINGLETON_INIT(CSLNetModule);

ISLNet* SLAPI sl::network::SLNetGetModule()
{
	return CSLNetModule::getSingletonPtr();
}

void CSLNetModule::registerExtMessageHandler(MessageID msgID, msgHandlerCB& handlerCB)
{
	MessageHandler* msgHander = new MessageHandler();
	msgHander->msgID = msgID;
	msgHander->msgCB = handlerCB;
	m_extMessageHandlers.add(msgHander);
}

void CSLNetModule::registerIntMessageHandler(MessageID msgID, msgHandlerCB& handlerCB)
{
	MessageHandler* msgHander = new MessageHandler();
	msgHander->msgID = msgID;
	msgHander->msgCB = handlerCB;
	m_intMessageHandlers.add(msgHander);
}

ISLListener* CSLNetModule::createListener()
{
	CSLListener* poListener = new CSLListener();
	return poListener;
}


}