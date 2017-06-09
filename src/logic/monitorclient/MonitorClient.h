#ifndef __SL_MONITOR_CLIENT_H__
#define __SL_MONITOR_CLIENT_H__
#include "slmulti_sys.h"
#include "slimodule.h"
#include "slsingleton.h"
#include "slbinary_stream.h"
#include <unordered_map>
#include <functional>

class IClient;
class MonitorClient : public sl::api::IModule, public sl::SLHolder<MonitorClient>{
	typedef std::function<void(sl::api::IKernel* pKernel, const int64 id, const sl::OBStream& args)>  MONITOR_CB;
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

private:
	void sendToSvr(sl::api::IKernel* pKernel, const int64 id, const int32 msgId, const sl::OBStream& buf);

private:
	IClient*	_client;

	std::unordered_map<int32, MONITOR_CB> _svrProtos;
};
#endif