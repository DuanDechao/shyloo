#ifndef __SL_CORE_REDIS_H__
#define __SL_CORE_REDIS_H__
#include "IRedis.h"
#include "slsingleton.h"
class Redis : public IRedis, public sl::SLHolder<Redis>{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual bool Exec(const int64 id, const char* command, const OArgs& args, const std::function<bool(sl::api::IKernel *, const sl::db::ISLRedisResult *)>& f = nullptr);
	virtual bool Call(const int64 id, const char* proc, const int32 keyCount, const OArgs& args, const std::function<bool(sl::api::IKernel *, const sl::db::ISLRedisResult *)>& f = nullptr);
};
#endif