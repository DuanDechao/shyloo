#ifndef _SL_INTERFACE_CLUSTER_H__
#define _SL_INTERFACE_CLUSTER_H__
#include "slimodule.h"
class IServerProcessHandler{
public:
	virtual ~IServerProcessHandler(){}

	virtual bool onServerReady(sl::api::IKernel* pKernel) = 0;
	virtual bool onServerReadyForLogin(sl::api::IKernel* pKernel) = 0;
	virtual bool onServerReadyForShutDown(sl::api::IKernel* pKernel) = 0;
	virtual bool onServerShutDown(sl::api::IKernel* pKernel) = 0;
};

class ICluster : public sl::api::IModule{
public:
	virtual ~ICluster() {}

	//all node interface
	virtual void addServerProcessHandler(IServerProcessHandler* handler) = 0;

	//master interface
	virtual int32 getBestNodeId(int32 nodeType) = 0;
	virtual int32 getNodeState(int32 nodeType, int32 nodeId) = 0;
};

#endif
