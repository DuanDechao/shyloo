#ifndef __SL_LIB_SLREDIS_H__
#define __SL_LIB_SLREDIS_H__
#include "slredis.h"
#include "hiredis.h"
#include <string>
#include "slstring.h"

namespace sl{
namespace db{
#define MAX_PASSWD_LEN 128
#define MAX_IP_LEN 128
class SLRedisMgr; 
class SLRedisConnection: public ISLRedisConnection{
public:
	SLRedisConnection(SLRedisMgr* redisMgr, const char* ip, const int32 port, const char* passwd, const int32 timeOut);
	~SLRedisConnection();
	virtual bool SLAPI reconnect();
	virtual void SLAPI release();
	virtual bool SLAPI isActive();
	virtual const char* SLAPI getIp(){ return _ip.c_str(); }
	virtual int32 SLAPI getPort() { return _port; }
	virtual bool SLAPI exec(char* command, const int32 size, const std::function<bool(ISLRedisResult* result)>& f);
	bool ping();

private:
	SLRedisMgr*		_redisMgr;
	int32			_port;
	redisContext*	_ctx;
	int32			_timeOut;
	sl::SLString<MAX_PASSWD_LEN> _passwd;
	sl::SLString<MAX_IP_LEN> _ip;
};
}
}

#endif