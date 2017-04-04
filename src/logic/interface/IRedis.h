#ifndef __SL_INTERFACE_IREDIS_H__
#define __SL_INTERFACE_IREDIS_H__
#include "slredis.h"
#include "slimodule.h"
#include "slikernel.h"
class OArgs;
class IRedis : public sl::api::IModule{
public:
	virtual ~IRedis() {}
	virtual bool Exec(const int64 id, const char* command, const OArgs& args, const std::function<bool(sl::api::IKernel *, const sl::db::ISLRedisResult *)>& f = nullptr) = 0;
	virtual bool Call(const int64 id, const char* proc, const int32 keyCount, const OArgs& args, const std::function<bool(sl::api::IKernel *, const sl::db::ISLRedisResult *)>& f = nullptr) = 0;
};
#endif