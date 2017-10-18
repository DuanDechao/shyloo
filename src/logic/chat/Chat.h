#ifndef __SL_LOGIC_CHAT_H__
#define __SL_LOGIC_CHAT_H__
#include "IChat.h"
#include "slstring.h"
#include <unordered_map>

class IEventEngine;
class IHarbor;
class IPlayerMgr;
class Chat : public IChat, public sl::api::ITcpSession, public sl::api::ITimer{
public:
	typedef std::function<void(sl::api::IKernel* pKernel, const void* context, const int32 size)> ChatMsgCB;

	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	void onPlayerOnline(sl::api::IKernel* pKernel, const void* context, const int32 size);
	void onRelationChannelSync(sl::api::IKernel* pKernel, const void* context, const int32 size);

	void onRelationChatChannelSync(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);

	virtual int32 onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen);
	virtual void onConnected(sl::api::IKernel* pKernel);
	virtual void onDisconnect(sl::api::IKernel* pKernel);

	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick){}

private:
	void syncChatChannel(sl::api::IKernel* pKernel, const char* ip, const int32 port);

private:
	Chat*				_self;
	sl::api::IKernel*	_kernel;
	IEventEngine*		_eventEngine;
	IHarbor*			_harbor;
	IPlayerMgr*			_playerMgr;

	sl::SLString<game::MAX_IP_LEN> _channelIP;
	int32				_channelPort;
	bool				_bConnected;

	std::unordered_map<int32, ChatMsgCB> _allCBPool;
};
#define RGS_NODE_HANDLER(chat, messageId, handler) chat->rgsNodeMessageHandler(messageId, std::bind(&handler, this, std::placeholders::_1, std::placeholders::_2,  std::placeholders::_3, std::placeholders::_4))

#endif