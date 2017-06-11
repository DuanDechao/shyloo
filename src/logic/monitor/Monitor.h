#ifndef __SL_MONITOR_H__
#define __SL_MONITOR_H__
#include "slmulti_sys.h"
#include "slsingleton.h"
#include "IMonitor.h"
#include "IAgent.h"
#include <unordered_map>

class IMonitorMessageHandler{
public:
	virtual ~IMonitorMessageHandler(){}
	virtual void DealMonitorMessage(sl::api::IKernel*, const int64, const char* pContext, const int32 size) = 0;
};

class IAgent;
class IHarbor;
class Monitor : public IMonitor, public IAgentListener, public sl::SLHolder<Monitor>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void rgsMonitorMessageHandler(int32 messageId, const MONITOR_CB& handler, const char* debug);
	virtual void rgsMonitorArgsMessageHandler(int32 messageId, const MONITOR_ARGS_CB& handler, const char* debug);

	virtual void onAgentOpen(sl::api::IKernel* pKernel, const int64 id);
	virtual void onAgentClose(sl::api::IKernel* pKernel, const int64 id);
	virtual int32 onAgentRecv(sl::api::IKernel* pKernel, const int64 id, const void* context, const int32 size);

private:
	std::unordered_map<int32, IMonitorMessageHandler*> _monitorProtos;
	IAgent*		_agent;
	IHarbor*	_harbor;
};
#endif