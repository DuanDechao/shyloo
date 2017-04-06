#define SL_DLL_EXPORT
#include "slredis_mgr.h"
#include "slredis_connection.h"
namespace sl{
namespace db{
ISLRedisConnection* SLRedisMgr::create(const char* ip, const int32 port, const int32 timeout){
	ISLRedisConnection* newConn = CREATE_FROM_POOL(_redisConnPool, this, ip, port, timeout);
	if (!newConn->reconnect()){
		SLASSERT(false, "connect redis failed");
		newConn->release();
		return nullptr;
	}
	return newConn;
}

void SLRedisMgr::recover(ISLRedisConnection* conn){
	if (conn)
		_redisConnPool.recover((SLRedisConnection*)conn);
}

extern "C" SL_DLL_API ISLRedisMgr* SLAPI getSLRedisMgr(void){
	return NEW SLRedisMgr();
}
}
}
