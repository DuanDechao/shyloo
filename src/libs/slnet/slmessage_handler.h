#ifndef _SL_MESSAGE_HANDLER_H_
#define _SL_MESSAGE_HANDLER_H_
#include "slmemorystream.h"
#include "slsmartpointer.h"
#include "slnetbase.h"

namespace sl
{
namespace network
{
class Channel;
class MessageHandlers;

//一個消息的參數抽象類
class MessageArgs
{
public:
	enum MESSAGE_ARGS_TYPE
	{
		MESSAGE_ARGS_TYPE_VARIABLE = -1,		// 可变参数长度
		MESSAGE_ARGS_TYPE_FIXED = 0				// 固定参数长度
	};

	MessageArgs():strArgsTypes(){};
	virtual ~MessageArgs(){};
	virtual void createFromStream(MemoryStream& s) = 0;
	virtual void addToStream(MemoryStream& s) = 0;
	virtual int32 dataSize(void) = 0;
	virtual MessageArgs::MESSAGE_ARGS_TYPE type(void){ return MESSAGE_ARGS_TYPE_FIXED; }

	std::vector<std::string> strArgsTypes;
};

struct ExposedMessageInfo
{
	std::string name;
	network::MessageID id;
	int16 msgLen;
	int8 argsType;
	std::vector<uint8> argsTypes;
};

class MessageHandler
{
public:
	MessageHandler();
	virtual ~MessageHandler();

	std::string name;
	MessageID	msgID;
	MessageArgs* pArgs;
	int32		msgLen;		///< 如果長度為-1則為非固定長度消息
	bool		exposed;
	MessageHandlers* pMessageHandlers;

	///stats
	volatile mutable uint32 send_size;
	volatile mutable uint32 send_count;
	volatile mutable uint32 recv_size;
	volatile mutable uint32 recv_count;

	uint32 sendsize() const {return send_size;}
	uint32 sendcount() const {return send_count;}
	uint32 sendavgsize() const {return (send_count <= 0) ? 0: send_size / send_count;}
	uint32 recvsize() const {return recv_size;}
	uint32 recvcount() const {return recv_count;}
	uint32 recvargsize() const {return (recv_count <=0)? 0: recv_size /recv_count;}

	//默認返回類別是組件消息
	virtual NETWORK_MESSAGE_TYPE type() const
	{
		return NETWORK_MESSAGE_TYPE_COMPONENT;
	}

	virtual int32 msglenMax(){return NETWORK_MESSAGE_MAX_SIZE;}

	const char* c_str();

	//當這個handler被正式安裝到MessageHandlers后被調用
	virtual void onInstall(){}

	virtual void handle(Channel* pChannel, MemoryStream& s)
	{
		pArgs->createFromStream(s);
	}
};

class MessageHandlers
{
public:
	static network::MessageHandlers* pMainMessageHandlers;
	typedef std::map<MessageID, MessageHandler*> MessageHandlerMap;

	MessageHandlers();
	~MessageHandlers();

	MessageHandler* add(std::string ihName, MessageArgs* args, int32 msgLen, MessageHandler* msgHandler);
	bool pushExposedMessage(std::string msgname);

	MessageHandler* find(MessageID msgID);

	MessageID lastMsgID() {return m_msgID = -1;}

	bool initializeWatcher();

	static void finalise(void);

	static std::vector<MessageHandlers*>& messageHandlers();

	const MessageHandlerMap& mapHandlers(){return m_msgHandlers;}

	static std::string getDigestStr();
private:
	MessageHandlerMap			m_msgHandlers;
	MessageID					m_msgID;
	std::vector<std::string>	m_exposedMessages;
};
}
}
#endif