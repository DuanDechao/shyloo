#ifndef _SL_INTERFACE_PACKETSENDER_H__
#define _SL_INTERFACE_PACKETSENDER_H__
#include "slimodule.h"
#include <unordered_map>
#include <vector>
#include "slbinary_stream.h"

class IPacketSender : public sl::api::IModule{
public:
	virtual ~IPacketSender() {}
	virtual void send(int32 gate, int64 id, int32 messageId, const void* context, const int32 size, int8 delay = 0) = 0;
	virtual void send(int32 gate, int64 id, int32 messageId, const sl::OBStream& args, int8 delay = 0) = 0;

	virtual void broadcast(const std::unordered_map<int32, std::vector<int64>>& gateIds, int32 messageId, const void* context, const int32 size, int8 delay = 0) = 0;
	virtual void broadcast(const std::unordered_map<int32, std::vector<int64>>& gateIds, int32 messageId, const sl::OBStream& args, int8 delay = 0) = 0;

	virtual void broadcast(int32 messageId, const void* context, const int32 size, int8 delay = 0) = 0;
	virtual void broadcast(int32 messageId, const sl::OBStream& args, int8 delay = 0) = 0;

};

#endif