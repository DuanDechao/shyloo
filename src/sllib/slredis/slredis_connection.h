#ifndef __SL_LIB_SLREDIS_H__
#define __SL_LIB_SLREDIS_H__
#include "slredis.h"
#include "hiredis.h"
#include <string>

namespace sl{
namespace db{
class SLRedisMgr; 
class SLRedisConnection: public ISLRedisConnection{
public:
	SLRedisConnection(SLRedisMgr* redisMgr, const char* ip, const int32 port, const int32 timeOut);
	~SLRedisConnection();
	virtual bool SLAPI reconnect();
	virtual void SLAPI release();
	virtual bool SLAPI isActive();
	virtual const char* SLAPI getIp(){ return _ip; }
	virtual int32 SLAPI getPort() { return _port; }
	virtual bool SLAPI exec(char* command, std::function<bool(ISLRedisResult* result)>& f);
	bool ping();

private:
	SLRedisMgr*		_redisMgr;
	char			_ip[256];
	int32			_port;
	redisContext*	_ctx;
	int32			_timeOut;
};
}
}

#endif