#include "slmessage_handler.h"
#include "slchannel.h"
#include "slnetwork_interface.h"
#include "slpacket_receiver.h"
#include "../common/slcommon.h"
#include "slfixed_messages.h"
namespace sl
{
namespace network
{
network::MessageHandlers* MessageHandlers::pMainMessageHandlers = 0;
std::vector<MessageHandlers*>* g_pMessageHandlers;

static network::FixedMessages* g_fm;

MessageHandlers::MessageHandlers()
	:m_msgHandlers(),
	 m_msgID(1),
	 m_exposedMessages()
{
	g_fm = network::FixedMessages::getSingletonPtr();
	network::FixedMessages::getSingletonPtr()->loadConfig("server/messages_fixed.xml");
	messageHandlers().push_back(this);
}

MessageHandlers::~MessageHandlers()
{
	MessageHandlerMap::iterator iter = m_msgHandlers.begin();
	for (; iter != m_msgHandlers.end(); ++iter)
	{
		if(iter->second)
			delete iter->second;
	}
}

MessageHandler::MessageHandler()
	:pArgs(NULL),
	 pMessageHandlers(NULL),
	 send_size(0),
	 send_count(0),
	 recv_size(0),
	 recv_count(0)
{}

MessageHandler::~MessageHandler()
{
	SAFE_RELEASE(pArgs);
}

const char* MessageHandler::c_str()
{
	static char buf[MAX_BUF];
	CPlatForm::_sl_snprintf(buf, MAX_BUF, "id:%u, len:%d", msgID, msgLen);
	return buf;
}

bool MessageHandlers::initializeWatcher()
{
	std::vector<std::string>::iterator siter = m_exposedMessages.begin();
	for (; siter != m_exposedMessages.end(); ++siter)
	{
		MessageHandlerMap::iterator iter = m_msgHandlers.begin();
		for (; iter != m_msgHandlers.end(); ++iter)
		{
			if((*siter) == iter->second->name)
			{
				iter->second->exposed = true;
			}
		}
	}

	MessageHandlerMap::iterator iter = m_msgHandlers.begin();
	for (; iter != m_msgHandlers.end(); ++iter)
	{
		char buf[MAX_BUF * 2];
		CPlatForm::_sl_snprintf(buf, MAX_BUF*2, "network/messages/%s/id", iter->second->name.c_str());

	}
	return true;
}
MessageHandler* MessageHandlers::add(std::string ihName, MessageArgs* args, int32 msgLen, MessageHandler* msgHandler)
{
	if(m_msgID == 1)
	{

	}

	FixedMessages::MSGInfo* msgInfo = FixedMessages::getSingletonPtr()->isFixed(ihName.c_str());
	if(msgInfo == NULL)
	{
		while(true)
		{
			if(FixedMessages::getSingletonPtr()->isFixed(m_msgID))
			{
				m_msgID++;
			}
			else
			{
				break;
			}
		}

		msgHandler->msgID = m_msgID++;
	}
	else
	{
		msgHandler->msgID = msgInfo->msgid;
	}

	return msgHandler;
}

MessageHandler* MessageHandlers::find(MessageID msgID)
{
	MessageHandlerMap::iterator iter = m_msgHandlers.find(msgID);
	if(iter != m_msgHandlers.end())
	{
		return iter->second;
	}
	return NULL;
}

std::vector<MessageHandlers*>& MessageHandlers::messageHandlers()
{
	if(g_pMessageHandlers == NULL)
		g_pMessageHandlers = new std::vector<MessageHandlers*>;

	return *g_pMessageHandlers;
}

void MessageHandlers::finalise()
{
	SAFE_RELEASE(g_pMessageHandlers);
}

bool MessageHandlers::pushExposedMessage(std::string msgname)
{
	m_exposedMessages.push_back(msgname);
	return true;
}

}
}