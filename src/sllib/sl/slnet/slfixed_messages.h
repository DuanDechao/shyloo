#ifndef _SL_FIXED_NETWORK_MESSAGES_H_
#define _SL_FIXED_NETWORK_MESSAGES_H_
#include "slsingleton.h"
#include "slnetbase.h"
#include <string>
#include <unordered_map>
namespace sl
{
namespace network
{
/*
	�������м�Э�飨ǰ������֮�䣩����ǿ��Լ��
*/
class FixedMessages: public CSingleton<FixedMessages>
{
public:
	//�̶���Э�����ݽṹ
	struct MSGInfo
	{
		MessageID	msgid;
		std::string msgname;
	};

public:
	FixedMessages();
	~FixedMessages();

	bool loadConfig(std::string fileName){return true;}

	FixedMessages::MSGInfo* isFixed(const char* msgName){return nullptr;}
	bool isFixed(MessageID msgid){return true;}

public:
	typedef SLUnordered_map<std::string, MSGInfo> MSGINFO_MAP;

private:
	MSGINFO_MAP		m_infomap;
	bool			m_loaded;
};
}
}
#endif