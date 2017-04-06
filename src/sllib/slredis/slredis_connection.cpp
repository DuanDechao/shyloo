#include "slredis_connection.h"
#include "slredis_callresult.h"
#include "slredis_mgr.h"
namespace sl{
namespace db{
SLRedisConnection::SLRedisConnection(SLRedisMgr* redisMgr, const char* ip, const int32 port, const int32 timeOut)
	:_redisMgr(redisMgr),
	_port(port),
	_ctx(nullptr),
	_timeOut(timeOut)
{
	safeMemcpy(_ip, sizeof(_ip), ip, strlen(ip));
}

SLRedisConnection::~SLRedisConnection(){
	if (_ctx){
		redisFree(_ctx);
		_ctx = nullptr;
	}
}

void SLRedisConnection::release(){
	SLASSERT(_redisMgr, "wtf");
	_redisMgr->recover(this);
}

bool SLRedisConnection::reconnect(){
	if (_ctx){
		redisFree(_ctx);
		_ctx = nullptr;
	}
	
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = _timeOut;
	_ctx = redisConnectWithTimeout(_ip, _port, tv);
	SLASSERT(_ctx, "wtf");
	if (!_ctx || _ctx->err){
		SLASSERT(false, "connect redis [%s:%d] failed", _ip, _port);
		if (_ctx)
			redisFree(_ctx);
		return false;
	}
	return true;
}

bool SLRedisConnection::isActive(){
	if (!_ctx || _ctx->err != REDIS_OK)
		return false;

	redisReply *reply = (redisReply*)redisCommand(_ctx, "ping");
	if (reply == NULL) 
		return false;

	if (reply->type != REDIS_REPLY_STATUS) 
		return false;

	if (stricmp(reply->str, "PONG") != 0) 
		return false;

	freeReplyObject(reply);
	return true;
}

bool SLRedisConnection::ping(){
	if (isActive())
		return true;
	return reconnect();
}

bool SLRedisConnection::exec(char* command, const std::function<bool(ISLRedisResult* result)>& f){
	if (!ping()){
		SLASSERT(false, "connect redis failed");
		return false;
	}

	_ctx->obuf = command;
	redisReply * reply = NULL;
	redisGetReply(_ctx, (void**)&reply);
	if (NULL == reply) {
		return false;
	}

	if (REDIS_REPLY_ERROR == reply->type || REDIS_REPLY_NIL == reply->type) {
		freeReplyObject(reply);
		return false;
	}

	bool ret = true;
	if (f) {
		SLRedisCallResult rst(reply);
		ret = f(&rst);
	}
	freeReplyObject(reply);

	return ret;
}

}
}