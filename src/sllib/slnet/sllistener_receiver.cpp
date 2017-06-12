#include "sllistener_receiver.h"
#include "sladdress.h"
#include "slbundle.h"
#include "slendpoint.h"
#include "slevent_dispatcher.h"
#include "slnetwork_interface.h"
#include "slpacket_receiver.h"

namespace sl
{
namespace network
{
ListenerReceiver::ListenerReceiver()
	:m_endpoint(nullptr),
	 m_networkInterface(nullptr),
	 m_pSessionFactory(nullptr),
	 m_pPacketParser(nullptr)
{}

ListenerReceiver::ListenerReceiver(EndPoint* endpoint, NetworkInterface* networkInterface)
	:m_endpoint(endpoint),
	 m_networkInterface(networkInterface),
	 m_pSessionFactory(nullptr),
	 m_pPacketParser(nullptr)
{}

ListenerReceiver::~ListenerReceiver(){
	m_endpoint = nullptr;
	m_networkInterface = nullptr;
	m_pSessionFactory = nullptr;
	m_pPacketParser = nullptr;
}

int ListenerReceiver::handleInputNotification(int fd)
{
	int tickcount = 0;
	while(tickcount++ <256)
	{
		EndPoint* pNewEndPoint = m_endpoint->accept();
		if(pNewEndPoint == NULL){
			break;
		}
		else{
			Channel* pChannel = CREATE_POOL_OBJECT(Channel, m_networkInterface, pNewEndPoint, m_pPacketParser);
			if (!pChannel){
				pChannel->destroy();
				RELEASE_POOL_OBJECT(Channel, pChannel);
				return 0;
			}

			if(!m_networkInterface->registerChannel(pChannel)){
				pChannel->destroy();
				RELEASE_POOL_OBJECT(Channel, pChannel);
			}

			//
			if(m_pSessionFactory == NULL){
				SLASSERT(false, "wtf");
				ECHO_ERROR("network inferface have no sessionfactory");
				return -1;
			}

			ISLSession* poSession = m_pSessionFactory->createSession(pChannel);
			if(NULL == poSession){
				ECHO_ERROR("create session failed");
				pChannel->destroy();
				RELEASE_POOL_OBJECT(Channel, pChannel);
				return -2;
			}
			if(!pChannel->isConnected()){
				pChannel->setSession(poSession);
				poSession->setChannel(pChannel);
				poSession->onEstablish();
				pChannel->setConnected();
			}
		}
	}
	return 0;
}
void ListenerReceiver::setSessionFactory(ISLSessionFactory* poSessionFactory){
	m_pSessionFactory = poSessionFactory;
}

void ListenerReceiver::setPacketParser(ISLPacketParser* poPacketParser){
	m_pPacketParser = poPacketParser;
}

}
}