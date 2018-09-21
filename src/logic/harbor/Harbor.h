#ifndef SL_LOGIC_HARBOR_H
#define SL_LOGIC_HARBOR_H
#include "IHarbor.h"
#include "slikernel.h"
#include "NodeSession.h"
#include <list>
#include <string>
#include "slsingleton.h"
#include "slshm.h"
class Harbor;
class NodeSessionTcpServer : public sl::api::ITcpServer{
public:
	NodeSessionTcpServer(Harbor* pHarbor) :_harbor(pHarbor){}
	virtual ~NodeSessionTcpServer(){}
	virtual sl::api::ITcpSession* mallocTcpSession(sl::api::IKernel* pKernel);
	virtual void setListenPort(uint16 port);

private:
	Harbor* _harbor;
};

class NodeSessionIpcServer : public sl::api::ITcpServer{
public:
	NodeSessionIpcServer(Harbor* pHarbor) :_harbor(pHarbor){}
	virtual ~NodeSessionIpcServer(){}
	virtual sl::api::ITcpSession* mallocTcpSession(sl::api::IKernel* pKernel);
	virtual void setListenPort(uint16 port){}

private:
	Harbor* _harbor;
};


class INodeMessageHandler{
public:
	virtual ~INodeMessageHandler(){}
	virtual void DealNodeMessage(sl::api::IKernel*, const int32, const int32, const char* pContext, const int32 size) = 0;
};

class Harbor: public IHarbor, public sl::SLHolder<Harbor>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual const int32 getNodeType() const { return _nodeType; }
	virtual const char* getNodeTypeStr() const {return s_nodeNames[_nodeType].c_str();}
	virtual const int32 getNodeId() const { return _nodeId; }
	virtual const int32 getPort() const { return _port; }
	virtual void setPort(uint16 port) {_port = port;}
	virtual void setLocalNodeId(int32 nodeId) {_nodeId = nodeId;}
	virtual bool isNodeConnected(const int32 nodeType, const int32 nodeId);
	
	virtual void onNodeOpen(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const char* ip, int32 nodePort, NodeSession* session);
	virtual void onNodeClose(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId);
	virtual void onNodeMessage(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const char* pszBuf, const int32 size);
	virtual void addNodeListener(INodeListener* pNodeListener);
	
	virtual void prepareSend(int32 nodeType, int32 nodeId, int32 messageId, int32 size);
	virtual void send(int32 nodeType, int32 nodeId, const void* pContext, const int32 size);
	virtual void send(int32 nodeType, int32 nodeId, int32 messageId, const OArgs& args);
	virtual void send(int32 nodeType, int32 nodeId, int32 messageId, const sl::OBStream& args);
	virtual void connect(const char* ip, const int32 port, const int32 nodeType, const int32 nodeId, bool isIpcTransfor = false);

	virtual void broadcast(int32 nodeType, int32 messageId, const OArgs& args);
	virtual void broadcast(int32 nodeType, int32 messageId, const sl::OBStream& args);
	virtual void broadcast(int32 messageId, const OArgs& args, std::unordered_map<int32, std::set<int32>>& exclude);
	virtual void broadcast(int32 messageId, const sl::OBStream& args, std::unordered_map<int32, std::set<int32>>& exclude);
	virtual void broadcast(int32 messageId, const OArgs& args);
	virtual void broadcast(int32 messageId, const sl::OBStream& args);
	virtual void prepareBroadcast(int32 nodeType, const int32 messageId, const int32 size);
	virtual void broadcast(int32 nodeType, const void* context, const int32 size);

	virtual void rgsNodeArgsMessageHandler(int32 messageId, const NodeArgsCB& handler);
	virtual void rgsNodeMessageHandler(int32 messageId, const NodeCB& handler);
	virtual void startListening(sl::api::IKernel* pKernel);

	virtual const char* getNodeName(int32 nodeType){ return s_nodeNames[nodeType].c_str(); }
	inline bool useIpc() const { return _useIpc; }
	int32 allocNodeIdByType(const int32 nodeType);

private:
	static bool initNodeTypes(sl::api::IKernel* pKernel);

private:
	sl::api::IKernel*	_pKernel;
	NodeSessionTcpServer*	_pTcpServer;
	NodeSessionIpcServer*   _pIpcServer;
	int32				_nodeType;
	int32				_nodeId;
	int32				_port;
	bool				_canAllocNodeId;
	std::unordered_map<int32, std::unordered_map<int32, NodeSession*>> _allNode;
	std::unordered_map<int32, std::list<INodeMessageHandler *>> _allCBPool;
	std::list<INodeListener*>	_listenerPool;
	bool						_useIpc;
	static std::unordered_map<std::string, int32> s_nodeTypes;
	static std::unordered_map<int32, std::string> s_nodeNames;
	std::unordered_map<int32, int32> _lastAllocNodeId;
	std::unordered_map<int32, uint64> _connectedNodes;
};
#endif
