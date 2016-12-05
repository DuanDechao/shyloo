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
ListenerReceiver::ListenerReceiver(EndPoint& endpoint, Channel::Traits traits, 
								   NetworkInterface& networkInterface)
								   :m_endpoint(endpoint),
								    m_traits(traits),
									m_networkInterface(networkInterface)
{}

ListenerReceiver::~ListenerReceiver(){}

int ListenerReceiver::handleInputNotification(int fd)
{
	int tickcount = 0;

	while(tickcount++ <256)
	{
		EndPoint* pNewEndPoint = m_endpoint.accept();
		if(pNewEndPoint == NULL)
		{
			if(tickcount == 1)
			{

			}
			break;
		}
		else
		{
			Channel* pChannel = Channel::createPoolObject();
			bool ret = pChannel->initialize(m_networkInterface, pNewEndPoint, m_traits);
			if(!ret)
			{
				pChannel->destroy();
				Channel::reclaimPoolObject(pChannel);
				return 0;
			}

			if(!m_networkInterface.registerChannel(pChannel))
			{
				pChannel->destroy();
				Channel::reclaimPoolObject(pChannel);
			}

			//
			if(m_pSessionFactory == NULL){
				SLASSERT(false, "wtf");
				ECHO_ERROR("network inferface have no sessionfactory");
				return -1;
			}

			ISLSession* poSession = m_pSessionFactory->createSession(pChannel);
			if(NULL == poSession)
			{
				ECHO_ERROR("create session failed");
				pChannel->destroy();
				Channel::reclaimPoolObject(pChannel);
				return -2;
			}
			if(!pChannel->isConnected()){
				pChannel->setSession(poSession);
				poSession->setChannel(pChannel);
				poSession->onEstablish();
				pChannel->setConnected(true);
			}
		}
	}
	return 0;
}
void ListenerReceiver::setSessionFactory(ISLSessionFactory* poSessionFactory){
	m_pSessionFactory = poSessionFactory;
}

EventDispatcher& ListenerReceiver::dispatcher()
{
	return m_networkInterface.getDispatcher();
}
}
}