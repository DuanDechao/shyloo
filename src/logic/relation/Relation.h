#ifndef __SL_FRAMEWORK_RELATION_H__
#define __SL_FRAMEWORK_RELATION_H__
#include "IRelation.h"
#include "slsingleton.h"
#include "slstring.h"
#include "GameDefine.h"
#include "IHarbor.h"

class IEventEngine;
class IObjectLocator;
class IPacketSender;
class IEventEngine;
class Relation : public IRelation, public sl::api::ITcpSession, public INodeListener, public sl::SLHolder<Relation>{
	enum{
		STATUS_NOT_GET_CHANNEL = 0,
		STATUS_HAS_GET_CHANNEL,
	};
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void sendToClient(int64 actorId, int32 messageId, const sl::OBStream& args);

	virtual int32 onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen);
	virtual void onConnected(sl::api::IKernel* pKernel){}
	virtual void onDisconnect(sl::api::IKernel* pKernel){}

	virtual void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port);
	virtual void onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId);

private:
	Relation*			_self;
	sl::api::IKernel*	_kernel;
	sl::SLString<game::MAX_IP_LEN> _chatBalanceIp;
	int32				_chatBalancePort;
	sl::SLString<game::MAX_IP_LEN> _channelIp;
	int32				_channelPort;
	int8				_status;
};
#endif