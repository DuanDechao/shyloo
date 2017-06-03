#ifndef _SL_IGATE_H__
#define _SL_IGATE_H__
#include "slikernel.h"
#include "slimodule.h"
#include <functional>
#include "slbinary_stream.h"

typedef std::function<void(sl::api::IKernel* pKernel, const int64 id, const char* pContext, const int32 size)> GATE_CB;
typedef std::function<void(sl::api::IKernel* pKernel, const int64 id, const sl::OBStream& args)> GATE_ARGS_CB;

class IGate : public sl::api::IModule{
public:
	virtual ~IGate() {}

	virtual void rgsGateMessageHandler(int32 messageId, const GATE_CB& handler, const char* debug) = 0;
	virtual void rgsGateArgsMessageHandler(int32 messageId, const GATE_ARGS_CB& handler, const char* debug) = 0;
};

#define RGS_GATE_HANDLER(gate, messageId, handler) gate->rgsGateMessageHandler(messageId, std::bind(&handler, this, std::placeholders::_1, std::placeholders::_2,  std::placeholders::_3, std::placeholders::_4), #handler)
#define RGS_GATE_ARGS_HANDLER(gate, messageId, handler) gate->rgsGateArgsMessageHandler(messageId, std::bind(&handler, this, std::placeholders::_1, std::placeholders::_2,  std::placeholders::_3), #handler)

#endif