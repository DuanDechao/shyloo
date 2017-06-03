#ifndef __SL_FRAMEWORK_LOGIC_H__
#define __SL_FRAMEWORK_LOGIC_H__
#include "slikernel.h"
#include "slimodule.h"
#include "ILogic.h"
#include "slsingleton.h"
#include <unordered_map>
#include <unordered_set>
#include "IHarbor.h"
#include "slstring.h"
#include "slbinary_stream.h"
#include "GameDefine.h"

class IObjectMgr;
class IEventEngine;
class IObject;
class IPlayerMgr;
class ICapacityPublisher;

class Logic :public ILogic, public INodeListener, public sl::SLHolder<Logic>{
	struct Handler{
		IProtocolHandler* _handler;
		sl::SLString<game::MAX_DEBUG_INFO_SIZE> _debug;

		bool operator == (const Handler& obj) const{
			return obj._handler == _handler;
		}
	};
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){}
	virtual void onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId);

	void onGateBindPlayerOnLogic(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onGateUnBindPlayerOnLogic(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onTransforMsgToLogic(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args);

protected:
	virtual void rgsProtocolHandler(int32 messageId, IProtocolHandler* handler, const char* debug);

private:
	void sendGateBindAck(sl::api::IKernel* pKernel, int32 nodeId, int64 accountId, int64 actorId, int32 errorCode);
	void sendSceneMgrBindNotify(sl::api::IKernel* pKernel, int64 accountId, int64 actorId, int32 errorCode);

private:
	Logic*		_self;
	IHarbor*	_harbor;
	IObjectMgr* _objectMgr;
	IPlayerMgr* _playerMgr;
	IEventEngine* _eventEngine;
	ICapacityPublisher*	_capacityPublisher;

	std::unordered_map<int32, std::unordered_set<int64>> _gateActors;
	std::unordered_map<int32, std::vector<Handler>> _protoHandlers;
};

#endif