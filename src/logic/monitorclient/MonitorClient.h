#ifndef __SL_MONITOR_CLIENT_H__
#define __SL_MONITOR_CLIENT_H__
#include "slmulti_sys.h"
#include "slimodule.h"
#include "slsingleton.h"
#include "slbinary_stream.h"
#include <unordered_map>
#include <functional>
#include "IClient.h"

class IClient;
class MonitorClient : public sl::api::IModule, public IClientListener, public sl::SLHolder<MonitorClient>{
	typedef std::function<void(sl::api::IKernel* pKernel, const sl::OBStream& args)>  MONITOR_CB;
	typedef std::function<void(sl::api::IKernel* pKernel)>  MONITOR_FUNC;

	enum{
		MONITOR_FUNC_SVR_SHUTDOWN = 0,
	};
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void rgsSvrMessageHandler(int32 messageId, const MONITOR_CB& handler);
	virtual void rgsMonitorFunc(int32 funcId, const MONITOR_FUNC& func);

	virtual void onServerConnected(sl::api::IKernel* pKernel);
	virtual void onServerDisConnected(sl::api::IKernel* pKernel);
	virtual int32 onServerMsg(sl::api::IKernel* pKernel, const void* context, const int32 size);

	void shutDownServer(sl::api::IKernel* pKernel);

private:
	void sendToSvr(sl::api::IKernel* pKernel, const int32 msgId, const sl::OBStream& buf);

private:
	IClient*	_client;

	std::unordered_map<int32, MONITOR_CB> _svrProtos;
	std::unordered_map<int32, MONITOR_FUNC> _monitorFunc;
};

#define RSG_MONITOR_CLIENT_HANDLER(_client, messageId, handler) _client->rgsSvrMessageHandler(messageId, std::bind(&handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
#define RSG_MONITOR_FUNC(_client, funcId, func) _client->rgsMonitorFunc(funcId, std::bind(&func, this, std::placeholders::_1))
#endif