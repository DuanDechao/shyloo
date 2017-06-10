#ifndef _SL_IMONITOR_H__
#define _SL_IMONITOR_H__
#include "slikernel.h"
#include "slimodule.h"
#include <functional>
#include "slbinary_stream.h"

typedef std::function<void(sl::api::IKernel* pKernel, const int64 id, const char* pContext, const int32 size)> MONITOR_CB;
typedef std::function<void(sl::api::IKernel* pKernel, const int64 id, const sl::OBStream& args)> MONITOR_ARGS_CB;

class IMonitor : public sl::api::IModule{
public:
	virtual ~IMonitor() {}

	virtual void rgsMonitorMessageHandler(int32 messageId, const MONITOR_CB& handler, const char* debug) = 0;
	virtual void rgsMonitorArgsMessageHandler(int32 messageId, const MONITOR_ARGS_CB& handler, const char* debug) = 0;
};

#define RGS_MONITOR_HANDLER(monitor, messageId, handler) monitor->rgsMonitorMessageHandler(messageId, std::bind(&handler, this, std::placeholders::_1, std::placeholders::_2,  std::placeholders::_3, std::placeholders::_4), #handler)
#define RGS_MONITOR_ARGS_HANDLER(monitor, messageId, handler) monitor->rgsMonitorArgsMessageHandler(messageId, std::bind(&handler, this, std::placeholders::_1, std::placeholders::_2,  std::placeholders::_3), #handler)

#endif