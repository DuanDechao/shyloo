#ifndef _IHARBOR_H_
#define _IHARBOR_H_
#include "slikernel.h"
#include "slimodule.h"
#include "slbinary_stream.h"
class OArgs;
class INodeListener{
public:
	virtual ~INodeListener() {}
	virtual void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port) = 0;
	virtual void onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId) = 0;
};

//typedef void(*node_cb)(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* pContext, const int32 size);
typedef std::function<void(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args)> NodeCB;
typedef std::function<void(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args)> NodeArgsCB;
class IHarbor : public sl::api::IModule{
public:
	virtual ~IHarbor() {}
	virtual void addNodeListener(INodeListener* pNodeListener) = 0;
	virtual void prepareSend(int32 nodeType, int32 nodeId, int32 messageId, int32 size) = 0;
	virtual void send(int32 nodeType, int32 nodeId, const void* pContext, const int32 size) = 0;
	virtual void send(int32 nodeType, int32 nodeId, int32 messageId, const OArgs& args) = 0;
	virtual void broadcast(int32 nodeType, int32 messageId, const OArgs& args) = 0;
	virtual void broadcast(int32 messageId, const OArgs& args) = 0;
	virtual void prepareBroadcast(int32 nodeType, const int32 messageId, const int32 size) = 0;
	virtual void broadcast(int32 nodeType, const void* context, const int32 size) = 0;
	virtual void rgsNodeArgsMessageHandler(int32 messageId, const NodeArgsCB& handler) = 0;
	virtual void rgsNodeMessageHandler(int32 messageId, const NodeCB& handler) = 0;
	virtual void connect(const char* ip, const int32 port, const int32 nodeType, const int32 nodeId, bool ipcTransfor = false) = 0;
	virtual int32 getNodeType() const = 0;
	virtual int32 getNodeId() const = 0;
	virtual const char* getNodeName(int32 nodeType) = 0;
};

#define RGS_NODE_HANDLER(harbor, messageId, handler) harbor->rgsNodeMessageHandler(messageId, std::bind(&handler, this, std::placeholders::_1, std::placeholders::_2,  std::placeholders::_3, std::placeholders::_4))
#define RGS_NODE_ARGS_HANDLER(harbor, messageId, handler) harbor->rgsNodeArgsMessageHandler(messageId, std::bind(&handler, this, std::placeholders::_1, std::placeholders::_2,  std::placeholders::_3, std::placeholders::_4))
#endif