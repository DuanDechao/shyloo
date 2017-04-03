#ifndef __SL_LIB_REDIS_MGR_H__
#define __SL_LIB_REDIS_MGR_H__
#include "slredis.h"
#include "slmulti_sys.h"
#include <string>
#include <vector>
#include "hiredis.h"
#include "slpool.h"
#include "slredis_connection.h"
namespace sl{
namespace db{
class SLRedisMgr: public ISLRedisMgr{
public:
	ISLRedisConnection* SLAPI create(const char* ip, const int32 port, const int32 timeout = CONNECT_TIME_OUT){
		ISLRedisConnection* newConn =  CREATE_FROM_POOL(_redisConnPool, this, ip, port, timeout);
		if (!newConn->reconnect()){
			SLASSERT(false, "connect redis failed");
			newConn->release();
			return nullptr;
		}
		return newConn;
	}

	void recover(ISLRedisConnection* conn){
		if (conn)
			_redisConnPool.recover((SLRedisConnection*)conn);
	}

	void SLAPI release(void) { DEL this; }

private:
	sl::SLPool<SLRedisConnection> _redisConnPool;
};
}
}

#endif