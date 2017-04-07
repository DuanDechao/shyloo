#ifndef __SL_CORE_REDIS_H__
#define __SL_CORE_REDIS_H__
#include "IRedis.h"
#include "slsingleton.h"
#include "slredis.h"
#include <unordered_map>
class Redis : public IRedis, public sl::SLHolder<Redis>{
	struct CommandBuf{
		int32 _size;
		char  _data[10240];
	};

	struct Context{
		sl::db::ISLRedisConnection* _conn;
		std::unordered_map<std::string, std::string> _scriptIds;
	};
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual bool exec(const int64 id, const char* command, const OArgs& args, const std::function<bool(sl::api::IKernel *, const sl::db::ISLRedisResult *)>& f = nullptr);
	virtual bool call(const int64 id, const char* proc, const int32 keyCount, const OArgs& args, const std::function<bool(sl::api::IKernel *, const sl::db::ISLRedisResult *)>& f = nullptr);

	void test();

private:
	bool loadScript(sl::api::IKernel* pKernel, const int64 id, const char* proc);

	void append(CommandBuf& buf, const OArgs& args);
	void append(CommandBuf& buf, int8 val);
	void append(CommandBuf& buf, int16 val);
	void append(CommandBuf& buf, int32 val);
	void append(CommandBuf& buf, int64 val);
	void append(CommandBuf& buf, float val);
	void append(CommandBuf& buf, const char* val);
	void append(CommandBuf& buf, const char* val, const int32 size);

private:
	sl::api::IKernel*						_kernel;
	sl::db::ISLRedisMgr*					_slRedisMgr;
	vector<Context>							_redisContexts;
	std::unordered_map<string, string>		_scripts;
};
#endif